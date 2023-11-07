let hdata = {
    x: [],
    type: 'histogram',
    nbinsx: 10
};

let layout = {
    title: "name",
    xaxis: {
        title: "name"
    },
    yaxis: {
        title: 'count'
    }
};

var jsonData;
var selectElement = document.getElementById('key');

function loadData() {
    const domain = window.location.origin.split(':')[1];
    const port = 1235;
    const url = `${domain}:${port}/api/fit_results`;
    fetch(url)
        .then(response => response.json())
        .then(data => {
            jsonData = data;
            if (jsonData && Array.isArray(jsonData) && jsonData.length > 0) {
                const event_data = jsonData[0];
                console.log(event_data);
                Object.keys(event_data).map(key => {
                    var opt = document.createElement("option");
                    opt.value = key;
                    opt.text = key;
                    selectElement.appendChild(opt);
                });
            }
        })
        .catch(error => {
            console.error('Error fetching JSON:', error);
        });
}

function onChangeKey() {
    var selectElement = document.getElementById('param_select')
    selectElement.setAttribute("hidden", "false");
}

document.getElementById('key').onchange = onChangeKey;
window.onload = loadData;

function plotData() {
    const channel = Number(document.getElementById('channel').value);
    hdata.nbinsx = Number(document.getElementById('nbins').value);
    const param_idx = Number(document.getElementById('param_idx').value);

    const ch_data = jsonData.filter((event) => { return Number(event.channel) === channel });
    var xdata = Array.from(ch_data, (event, id) => { return event.fit_params[param_idx]; });

    hdata.x = xdata;

    const param_name = 'param[' + String(param_idx) + ']';
    layout.title = param_name;
    layout.xaxis.title = param_name;

    Plotly.newPlot('plot', [hdata], layout);
}

document.getElementById('plot_button').onclick = plotData;