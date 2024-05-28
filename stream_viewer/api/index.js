// server.js
const express = require('express');
//const kafka = require('kafka-node');
const Kafka = require('node-rdkafka');
const bodyParser = require('body-parser');
//const arrow = require('@apache-arrow/es5-cjs');
const arrow = require('apache-arrow');
const app = express();
const cors = require('cors');
const port = process.env.PORT || 1235;

app.use(cors());
app.use(bodyParser.json());

let arrowBuffer;

// Kafka consumer configuration
const consumerConfig = {
    'group.id': 'arrow-consumer-group',
    'metadata.broker.list': '192.168.0.2:9092,192.168.0.9:9092',
    'enable.auto.commit': true,
    'auto.offset.reset': 'earliest',
};

// Create a Kafka consumer
const consumer = new Kafka.KafkaConsumer(consumerConfig, {});

// Handle connection errors
consumer.on('event.error', (err) => {
    console.error('Error from consumer', err);
});

// Handle messages
consumer.on('data', async (data) => {
    try {
        // Deserialize the Apache Arrow Table from the Kafka message
        const buffer = data.value;
        //const table = arrow.tableFromIPC(buffer);
        arrowBuffer = data.value;

        // Process the Arrow Table
        console.log('Received Arrow Table:');
        // You can now process the table as needed
    } catch (err) {
        console.error('Error processing message', err);
    }
});

// Connect the consumer
consumer.connect();

// Subscribe to the topic
consumer.on('ready', () => {
    console.log('Consumer ready');
    // Set the specific partition and offset to start from
    const topic = 'decoded';
    const partition = 0;  // Specify your partition
    const offset = 2020;  // Specify your desired offset

    // Assign the consumer to the specified partition and offset
    consumer.assign([{ topic, partition, offset }]);
    //consumer.subscribe(['decoded']);
    consumer.consume();
});


// Define a route to serve the waveform data
app.get('/api/waveform', (req, res) => {
    console.log('get access at /waveform');
    //res.send(arrowBuffer);
    const table = arrow.tableFromIPC(arrowBuffer);
    const dataRow0 = table.get(0)['data'];
    const eventId = table.get(0)['event_id'];
    waveformData = [];
    for (var i = 0; i < dataRow0.length; i++) {
        const ch = dataRow0.get(i)['ch'];
        const wave = dataRow0.get(i)['waveform'];
        waveform = [];
        for (var j = 0; j < wave.length; j++) {
            waveform.push(wave.get(j));
        }
        waveformData.push({ 'ch': ch, 'eventId': eventId, 'waveform': waveform });
    }
    res.json(JSON.stringify(waveformData));

});

//app.get('/api/adc', (req, res) => {
//    console.log('get access at /adc');
//    //res.send(arrowBuffer);
//    const table = arrow.tableFromIPC(arrowBuffer);
//    const dataRow0 = table.get(0)['data'];
//    const eventId = table.get(0)['event_id'];
//    waveformData = [];
//    for (var i = 0; i < dataRow0.length; i++) {
//        const ch = dataRow0.get(i)['ch'];
//        const wave = dataRow0.get(i)['waveform'];
//        waveform = [];
//        for (var j = 0; j < wave.length; j++) {
//            waveform.push(wave.get(j));
//        }
//        waveformData.push({ 'ch': ch, 'eventId': eventId, 'waveform': waveform });
//    }
//    res.json(JSON.stringify(waveformData));
//
//
//});
// Define a route to serve the fit output data
//app.get('/api/fit_results', (req, res) => {
//    const fs = require('fs');
//    fs.readFile('/home/daq/opt/mira-tools/work/fit_output.json', 'utf-8', (err, data) => {
//        res.json(JSON.parse(data));
//    });
//});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on port ${port}`);
});