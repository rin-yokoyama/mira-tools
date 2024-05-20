import React, { useEffect, useState } from 'react';
import { Table, tableFromIPC } from '@apache-arrow/es5-cjs';
import { RecordBatchReader } from '@apache-arrow/es5-cjs';
import { Chart } from 'react-google-charts';
import { channel } from 'process';

var json_data;
var event_size = 0;
var charts = [];

function loadData() {
    const domain = window.location.hostname;
    const port = 1235;
    const url = `http://${domain}:${port}/api/waveform`;

    return fetch(url)
        .then(response => response.json())
        .then(data => {
            json_data = JSON.parse(data);
            if (json_data && Array.isArray(json_data)) {
                event_size = json_data.length;
                Waveform();
            }
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        })
}

export default function Waveform() {
    const [chValue, setChValue] = useState('');
    const handleChChange = (event) => {
        setChValue(event.target.value);
        generateData();
    };
    const handleRefresh = (event) => {
        loadData();
        setChValue(0);
        generateData();
    };

    const generateData = () => {

        if (json_data) {
            charts = [];
            json_data.forEach(element => {
                const ydata = element.waveform;
                const ch = element.ch;
                const eventId = element.eventId;
                let xdata = arr = Array.from({ length: ydata.length }, (e, i) => [i, ydata[i]]);
                xdata.unshift(['x', 'y'])
                plotData = xdata;
                plotOption = {
                    title: 'waveform ch:' + String(ch) + ', event:' + String(eventId),
                    hAxis: { title: 'sample id' },
                    vAxis: { title: 'ADC' },
                    legend: 'none'
                };

                charts.push(
                    <td><Chart
                        width={'100%'}
                        height={'400px'}
                        chartType="LineChart"
                        loader={<div>Loading Chart</div>}
                        data={plotData}
                        options={plotOption}
                        rootProps={{ 'data-testid': '2' }}
                        key={ch}
                    /></td>
                );

            });
        }
    };

    const eventSize = event_size;
    generateData();

    return (
        <div id='waveform' >
            <label>Channel:
                <input type="number" id="chanel" name="channel" min="0" defaultValue={51} onChange={handleChChange}></input>
            </label>
            <br />
            <button onClick={handleRefresh}>Refresh</button>
            <tr>
                {charts}
            </tr>
        </div >
    );
}