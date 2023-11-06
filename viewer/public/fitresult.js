function plotData() {
    const domain = window.location.origin.split(':')[1];
    const port = 1235;
    const url = `${domain}:${port}/api/fit_results`;

    let param_idx = Number(document.getElementById('param_idx').value);
    let param_name = 'param[' + String(param_idx) + ']';
    let channel = Number(document.getElementById('channel').value);
    let nbins = Number(document.getElementById('nbins').value);

    fetch(url)
        .then(response => response.json())
        .then(data => {

            const ch_data = data.filter((event) => { return Number(event.channel) === channel });
            var xdata = Array.from(ch_data, (event, id) => { return event.fit_params[param_idx]; });

            let hdata = {
                x: xdata,
                type: 'histogram',
                nbinsx: nbins
            };

            let layout = {
                title: param_name,
                xaxis: {
                    title: param_name
                },
                yaxis: {
                    title: 'count'
                }
            };

            Plotly.newPlot('plot', [hdata], layout);
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        });

}

document.getElementById('plot_button').onclick = plotData;