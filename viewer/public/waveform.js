function plotData() {
    const domain = window.location.origin.split(':')[1];
    const port = 1235;
    const url = `${domain}:${port}/api`;

    const event_id = document.getElementById('event_id').value;
    const channel = document.getElementById('channel').value;

    fetch(url)
        .then(response => response.json())
        .then(data => {

            const event_data = data.find(element => Number(element.event_id) === Number(event_id));
            const channel_data = event_data.data.find((element) => Number(element.channel) === Number(channel));
            const ydata = channel_data.waveform;
            const xdata = arr = Array.from({ length: ydata.length }, (e, i) => i);
            const trace = {
                x: xdata,
                y: ydata,
                mode: 'markers',
                type: 'scatter'
            };

            const layout = {
                title: 'waveform',
                xaxis: {
                    title: 'sample id'
                },
                yaxis: {
                    title: 'adc'
                }
            };

            Plotly.newPlot('plot', [trace], layout);
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        });

}

document.getElementById('plot_button').onclick = plotData;