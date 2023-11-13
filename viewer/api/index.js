// server.js
const express = require('express');
const app = express();
const cors = require('cors');
const port = process.env.PORT || 1235;
app.use(cors());

// Define a route to serve the waveform data
app.get('/api/waveform', (req, res) => {
    const fs = require('fs');
    fs.readFile('/home/daq/opt/mira-tools/work/output.json', 'utf-8', (err, data) => {
        res.json(JSON.parse(data));
    });
});

// Define a route to serve the bg subtraction data
app.get('/api/subtracted', (req, res) => {
    const fs = require('fs');
    fs.readFile('/home/daq/opt/mira-tools/work/subtracted_output.json', 'utf-8', (err, data) => {
        res.json(JSON.parse(data));
    });
});

// Define a route to serve the fit output data
app.get('/api/fit_results', (req, res) => {
    const fs = require('fs');
    fs.readFile('/home/daq/opt/mira-tools/work/fit_output.json', 'utf-8', (err, data) => {
        res.json(JSON.parse(data));
    });
});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on port ${port}`);
});