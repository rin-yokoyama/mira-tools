import cupy as cp
import numpy as np
import matplotlib.pyplot as plot
import json

json_file = open('/home/daq/opt/mira-tools/work/output.json','r')
data = json.load(json_file)
qdc = np.zeros((5, 340))
dynode = np.zeros((1,340))
qsum = np.zeros((1,340))
xpos = np.zeros((1,340))
ypos = np.zeros((1,340))
xfiltered = np.empty((1))
yfiltered = np.empty((1))
yunfiltered = np.empty((1))
for evt in data:
    print(evt['event_id'])
    for ch_data in evt['data']:
        ch = ch_data['channel']
        waveform = ch_data['waveform']
        waveform_gpu = cp.asarray(np.array(waveform))
        fft_result = cp.fft.fft(waveform_gpu)
        fft_result_cpu = cp.asnumpy(fft_result)
        freq = np.fft.fftfreq(len(waveform))
        #fft_result_cpu[abs(abs(freq)-0.0060)<0.001] = 0
        fft_result_cpu[abs(freq)<0.007] = 0
        filtered_gpu = cp.fft.ifft(cp.asarray(fft_result_cpu)).real
        filtered_cpu = cp.asnumpy(filtered_gpu)
        baseline = np.sum(filtered_cpu[0:450])/450.
        qdc[ch][evt['event_id']-1] = np.sum(filtered_cpu[475:490]) - baseline
        if (evt['event_id'] == 1):
            xfiltered = np.array([i for i in range(len(filtered_cpu))])
            yfiltered = filtered_cpu
            yunfiltered = np.array(waveform)
            bline = np.sum(yunfiltered[0:450])/450.
            yunfiltered = yunfiltered - bline
    
dynode[0] = qdc[0]
qsum[0] = qdc[1] + qdc[2] + qdc[3] + qdc[4]
xpos[0] = (qdc[1] + qdc[3])/qsum[0]
ypos[0] = (qdc[4] + qdc[3])/qsum[0]
xpos[dynode<10] = -1
ypos[dynode<10] = -1

print(dynode)

plot.figure().set_figwidth(15)
plot.plot(xfiltered,yunfiltered)
plot.plot(xfiltered,yfiltered)
plot.legend(["unfiltered","filtered"])
plot.savefig('filtered.png', format='png')
plot.figure().set_figwidth(5)
plot.hist(dynode[0], 500)
plot.savefig('dynode.png', format='png')
print('save plot dynode')
plot.hist2d(xpos[0], ypos[0], [100,100],[[0,1],[0,1]])
plot.savefig('position.png', format='png')
print('save plot position')