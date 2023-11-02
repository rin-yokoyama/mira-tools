// server.js
const express = require('express');
const app = express();
const cors = require('cors');
const port = process.env.PORT || 1235;
app.use(cors());

// Your JSON data (replace this with your actual data)
const jsonData = {
    "x": [
        1,
        2,
        3,
        4
    ],
    "y": [
        1,
        1,
        2,
        2
    ]
};

// Define a route to serve the JSON data
app.get('/api', (req, res) => {
    res.json(jsonData);
});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on port ${port}`);
});