#include <iostream>
#include <fstream>
#include <sstream>
#include "mira_decoder.hpp"
#include <librdkafka/rdkafka.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: stream_decoder [bootstrap.servers] [client.id (= 0)]" << std::endl;
        return 1;
    }
    std::string bootstrap_servers = argv[1];
    std::string client_id = "0";
    if (argc > 2)
    {
        client_id = argv[2];
    }

    // Initialize a Kafka consumer
    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "client.id", client_id.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", "decoder", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "bootstrap.servers", bootstrap_servers.c_str(), errstr, sizeof(errstr));
    rd_kafka_t *rk;
    rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    rd_kafka_poll_set_consumer(rk);
    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, "rawdata", RD_KAFKA_PARTITION_UA);
    auto err = rd_kafka_subscribe(rk, topics);
    if (err)
        std::cout << rd_kafka_err2str(err) << std::endl;
    rd_kafka_topic_partition_list_destroy(topics);

    // polling loop
    while (true)
    {
        rd_kafka_message_t *rkmessage = rd_kafka_consumer_poll(rk, 500);
        if (rkmessage)
        {
            if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
                continue; // The partition is empty
            if (rkmessage->err)
            {
                std::cout << rd_kafka_err2str(rkmessage->err) << std::endl;
                break;
            }
            u_int64_t size = rkmessage->len;
            u_int32_t *buf32 = (u_int32_t *)rkmessage->payload;
            auto data = mira::decode_buffer(buf32, size / 4, {0, 1, 2, 3, 4});

            auto json_data = mira::event_data_to_json(data);
            std::cout << json_data << std::endl;

            rd_kafka_message_destroy(rkmessage);
        }
    }

    return 0;
}
