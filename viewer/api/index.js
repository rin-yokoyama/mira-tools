// server.js
const express = require('express');
const app = express();
const cors = require('cors');
const port = process.env.PORT || 1235;
app.use(cors());

var jsonData;
const fs = require('fs');
fs.readFile('/home/daq/opt/mira-tools/work/output.json', 'utf-8', (err, data) => {
    jsonData = JSON.parse(data);
});


// Define a route to serve the JSON data
app.get('/api', (req, res) => {
    res.json(jsonData);
});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on port ${port}`);
});