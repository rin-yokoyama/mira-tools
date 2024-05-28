import React, { useState } from 'react';
import { Chart } from 'react-google-charts';

var json_data;
var options = ['loading...'];
var plotData = [['loading...'], [0]];
var plotOption = {};

function loadData() {
    const domain = window.location.hostname;
    const port = 1235;
    const url = `http://${domain}:${port}/api/fit_results`;

    return fetch(url)
        .then(response => response.json())
        .then(data => {
            json_data = data;
            if (json_data && Array.isArray(json_data) && json_data.length > 0) {
                const event_data = json_data[0];
                if (event_data) {
                    options = Object.keys(event_data).map(key => (key));
                }
            }
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        })
}

export default function Waveform() {
    loadData();

    const [keyValue, setKeyValue] = useState('');
    const [idxValue, setIdxValue] = useState('');
    const [nbinsValue, setNbinsValue] = useState('');
    const [chValue, setChValue] = useState('');

    const handleKeyChange = (event) => {
        setKeyValue(event.target.value);
        generateData();
    };
    const handleIdxChange = (event) => {
        setIdxValue(event.target.value);
        generateData();
    };
    const handleNbinsChange = (event) => {
        setNbinsValue(event.target.value);
        generateData();
    };
    const handleChChange = (event) => {
        setChValue(event.target.value);
        generateData();
    };

    const generateData = () => {
        if (json_data) {
            const ch_data = json_data.filter((event) => { return Number(event.channel === Number(chValue)) });
            if (ch_data) {
                var xname;
                if (keyValue === 'fit_params' || keyValue === 'init_params') {
                    plotData = Array.from(ch_data, (event, id) => { return [event.fit_params[idxValue]] });
                    xname = keyValue + '[' + String(idxValue) + ']';
                    plotData.unshift([xname]);
                }
                else {
                    plotData = Array.from(ch_data, (event, id) => { return [event[keyValue]] });
                    xname = keyValue;
                    plotData.unshift([xname]);
                    console.log(plotData);
                }
                plotOption = {
                    title: xname + 'histogram',
                    hAxis: { title: xname },
                    vAxis: { title: 'counts' },
                    legend: 'none',
                    histogram: { bucketSize: nbinsValue }
                };
            }
        }
    };

    generateData();
    return (
        <div id='fitresults' >
            <label>key to plot:
                <select id="key" name="key" onChange={handleKeyChange}>
                    {options.map((option) => (
                        <option key={option} value={option}>{option}</option>
                    ))}
                </select>
            </label><br />
            <label>parameter index to plot:
                <select id="param_idx" name="param_idx" onChange={handleIdxChange}>
                    <option value="0">p0 (amplitude)</option>
                    <option value="1">p1 (peak time)</option>
                    <option value="2">p2 (rise time)</option>
                    <option value="3">p3 (decay time)</option>
                    <option value="4">p4 (baseline)</option>
                </select>
            </label><br />
            <label>Channel:
                <input type="number" id="channel" name="channel" min="0" defaultValue={0} onChange={handleChChange}></input>
            </label><br />
            <lable>bin width:
                <input id="nbins" name="nbins" defaultValue={1} onChange={handleNbinsChange}></input>
            </lable><br />

            <Chart
                width={'100%'}
                height={'400px'}
                chartType="Histogram"
                loader={<div>Loading Chart</div>}
                data={plotData}
                options={plotOption}
                rootProps={{ 'data-testid': '1' }}
            />
        </div >
    );
}