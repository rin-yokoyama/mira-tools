import React, { useState } from 'react';
import { Chart } from 'react-google-charts';

var json_data;
var event_size;
var plotData;
var plotOption;
var plotData2;
var plotOption2;

function loadData() {
    const domain = window.location.hostname;
    const port = 1235;
    const url = `http://${domain}:${port}/api/subtracted`;

    return fetch(url)
        .then(response => response.json())
        .then(data => {
            json_data = data;
            if (json_data && Array.isArray(json_data)) {
                event_size = json_data.event_id[json_data.length];
            }
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        })
}

export default function Subtracted() {
    loadData();

    const [sliderValue, setSliderValue] = useState('');
    const [chValue, setChValue] = useState('');
    const handleChange = (event) => {
        setSliderValue(event.target.value);
        generateData();
    };
    const handleChChange = (event) => {
        setChValue(event.target.value);
        generateData();
    };

    const generateData = () => {
        if (json_data) {
            const event_data = json_data.find(element => Number(element.event_id) === Number(sliderValue) && Number(element.channel) === Number(chValue));
            if (event_data) {
                const ydata = event_data.waveform;
                let xdata = arr = Array.from({ length: ydata.length }, (e, i) => [i, ydata[i]]);
                xdata.unshift(['x', 'y'])
                plotData = xdata;
                plotOption = {
                    title: 'subtracted waveform (evt: ' + String(sliderValue) + ')',
                    hAxis: { title: 'sample id' },
                    vAxis: { title: 'ADC' },
                    legend: 'none'
                };

                const ydata2 = event_data.function;
                let xdata2 = arr = Array.from({ length: ydata2.length }, (e, i) => [i, ydata2[i]]);
                xdata2.unshift(['x', 'y'])
                plotData2 = xdata2;
                plotOption2 = {
                    title: 'bg fit function (evt: ' + String(sliderValue) + ')',
                    hAxis: { title: 'sample id' },
                    vAxis: { title: 'ADC' },
                    legend: 'none',
                    colors: ['red']
                }
            }
        }
    };

    const eventSize = event_size;
    generateData();
    return (
        <div id='subtracted_waveform' >
            <label>Event id:
                <input type="range" id="event_id" name="event_id" min="1" max={eventSize} defaultValue={1} onChange={handleChange}></input>
                {sliderValue}
            </label><br />
            <label>Channel:
                <input type="number" id="chanel" name="channel" min="0" defaultValue={0} onChange={handleChChange}></input>
            </label>

            <Chart
                width={'100%'}
                height={'400px'}
                chartType="ScatterChart"
                loader={<div>Loading Chart</div>}
                data={plotData}
                options={plotOption}
                rootProps={{ 'data-testid': '2' }}
            />
            <Chart
                width={'100%'}
                height={'400px'}
                chartType="ScatterChart"
                loader={<div>Loading Chart</div>}
                data={plotData2}
                options={plotOption2}
                rootProps={{ 'data-testid': '2' }}
            />
        </div >
    );
}