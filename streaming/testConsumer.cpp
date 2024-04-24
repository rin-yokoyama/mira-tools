#include <iostream>
#include <bitset>
#include <librdkafka/rdkafka.h>

char hostname[128];
char errstr[512];

int main(int argc, char **argv)
{

    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "client.id", "0", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", "c", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "bootstrap.servers", "192.168.0.2:9092", errstr, sizeof(errstr));
    rd_kafka_t *rk;
    rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    rd_kafka_poll_set_consumer(rk);
    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, "test", RD_KAFKA_PARTITION_UA);
    auto err = rd_kafka_subscribe(rk, topics);
    if (err)
        std::cout << rd_kafka_err2str(err) << std::endl;
    rd_kafka_topic_partition_list_destroy(topics);

    while (true)
    {
        rd_kafka_message_t *rkmessage = rd_kafka_consumer_poll(rk, 500);
        // std::cout << rkmessage << std::endl;
        if (rkmessage)
        {
            if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
                continue;
            if (rkmessage->err)
                continue;
            // std::cout << rd_kafka_topic_name(rkmessage->rkt) << std::endl;
            std::cout << "len: " << rkmessage->len << ", ";
            // u_int8_t *payload = new u_int8_t[rkmessage->len];
            for (int i = 0; i < rkmessage->len; ++i)
                std::cout << std::hex << *(((u_int8_t *)rkmessage->payload) + i);
            // delete[] payload;
            std::cout << std::endl;
            rd_kafka_message_destroy(rkmessage);
        }
    }

    return 0;
}
