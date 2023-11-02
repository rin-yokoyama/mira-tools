function plotData() {
    const domain = window.location.origin.split(':')[1];
    const port = 1235;
    const url = `${domain}:${port}/api`;
    console.log(url);
    fetch(url)
        .then(response => response.json())
        .then(data => {

            const trace = {
                x: data.x,
                y: data.y,
                mode: 'markers',
                type: 'scatter'
            };

            const layout = {
                title: 'Parcel & Plotly Scatter Plot',
                xaxis: {
                    title: 'X Axis Label'
                },
                yaxis: {
                    title: 'Y Axis Label'
                }
            };

            Plotly.newPlot('plot', [trace], layout);
            document.getElementById('event_id').value = 2;
        })
        .catch(error => {
            document.getElementById('event_id').value = 1;
            console.error('Error fetching JSON:', error);
        });

}

document.getElementById('plot_button').onclick = plotData;