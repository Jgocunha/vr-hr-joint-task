% Launcher for the ICSR VR HRI architecture, ported from dynamic-neural-field-composer.
% Four one-dimensional circular fields:
%   aol - action observation layer (driven by the hand position stimulus)
%   orl - object memory layer      (driven by three object stimuli)
%   asl - action simulation layer  (driven by aol and orl)
%   ael - action execution layer   (inhibited by asl, excited by orl)
% Hover over sliders and buttons to see a description of their function.
%
% Notes on the port from dnf-composer (element dimensions {x_max = 50, d_x = 0.5}):
% - The fields have round(x_max / d_x) = 100 samples.
% - Kernel and stimulus widths were already in samples in dnf-composer and
%   are used unchanged. Stimulus positions were in spatial units and are
%   divided by d_x here. The position sliders show dnf-composer units (0-50).
% - Every dnf-composer GaussKernel / MexicanHatKernel becomes a
%   LateralInteractions1D, because cosivina's GaussKernel1D has no global
%   component. A plain Gauss kernel uses amplitudeInh = 0 (sigmaInh = 1 avoids
%   a 0 * NaN kernel).
% - dnf-composer NormalNoise uses amplitude / sqrt(deltaT), the same as
%   cosivina's NormalNoise, so it feeds the fields directly with no kernel.
% - Elements are added in the original order. Both tools step elements in
%   insertion order, so the update order is preserved.


%% setting up the simulator

% shared parameters
x_max = 50;
d_x = 0.5;
fieldSize = round(x_max / d_x);
deltaT = 10;

circularity = true;
normalization = false;
tau = 100;
resting_level = -5;
steepness = 4;
stimulus_sigma = 3;
stimulus_amplitude = 5;
noise_amplitude = 0.05;
no_inh_sigma = 1;

sim = Simulator('deltaT', deltaT);

% action observation layer
sim.addElement(GaussStimulus1D('hand position stimulus', fieldSize, stimulus_sigma + 1, 0, 0 / d_x, ...
  circularity, true));
sim.addElement(NeuralField('aol', fieldSize, tau, resting_level, steepness));
sim.addElement(LateralInteractions1D('aol -> aol', fieldSize, 1, 1.5, no_inh_sigma, 0, -0.05, ...
  circularity, normalization));
sim.addElement(NormalNoise('normal noise aol', fieldSize, noise_amplitude));

sim.addConnection('aol', 'output', 'aol -> aol');
sim.addConnection('aol -> aol', 'output', 'aol');
sim.addConnection('normal noise aol', 'output', 'aol');
sim.addConnection('hand position stimulus', 'output', 'aol');

% action simulation layer
sim.addElement(NeuralField('asl', fieldSize, tau, resting_level, steepness));
sim.addElement(LateralInteractions1D('asl -> asl', fieldSize, 1, 1.5, no_inh_sigma, 0, -0.05, ...
  circularity, normalization));
sim.addElement(LateralInteractions1D('aol -> asl', fieldSize, 3.0, 0.755, no_inh_sigma, 0, 0.0, ...
  circularity, normalization));
sim.addElement(NormalNoise('normal noise asl', fieldSize, noise_amplitude));

sim.addConnection('asl', 'output', 'asl -> asl');
sim.addConnection('asl -> asl', 'output', 'asl');
sim.addConnection('normal noise asl', 'output', 'asl');
sim.addConnection('aol', 'output', 'aol -> asl');
sim.addConnection('aol -> asl', 'output', 'asl');

% object memory layer
sim.addElement(GaussStimulus1D('object stimulus 3', fieldSize, stimulus_sigma, stimulus_amplitude, 12.5 / d_x, ...
  circularity, normalization));
sim.addElement(GaussStimulus1D('object stimulus 2', fieldSize, stimulus_sigma, stimulus_amplitude, 25 / d_x, ...
  circularity, normalization));
sim.addElement(GaussStimulus1D('object stimulus 1', fieldSize, stimulus_sigma, stimulus_amplitude, 37.5 / d_x, ...
  circularity, normalization));
sim.addElement(NeuralField('orl', fieldSize, tau, resting_level, steepness));
sim.addElement(LateralInteractions1D('orl -> orl', fieldSize, 1, 2, 0.5, 1.5, 0.0, ...
  circularity, normalization));
sim.addElement(LateralInteractions1D('orl -> asl', fieldSize, 1.9, 0.7, no_inh_sigma, 0, 0.0, ...
  circularity, normalization));
sim.addElement(NormalNoise('normal noise orl', fieldSize, noise_amplitude));

sim.addConnection('orl', 'output', 'orl -> asl');
sim.addConnection('orl -> asl', 'output', 'asl');
sim.addConnection('orl', 'output', 'orl -> orl');
sim.addConnection('orl -> orl', 'output', 'orl');
sim.addConnection('normal noise orl', 'output', 'orl');
sim.addConnection('object stimulus 1', 'output', 'orl');
sim.addConnection('object stimulus 2', 'output', 'orl');
sim.addConnection('object stimulus 3', 'output', 'orl');

% action execution layer
sim.addElement(NeuralField('ael', fieldSize, tau, resting_level, steepness));
sim.addElement(LateralInteractions1D('asl -> ael', fieldSize, 4, -2.5, no_inh_sigma, 0, 0.0, ...
  circularity, normalization));
sim.addElement(LateralInteractions1D('ael -> ael', fieldSize, 2.5, 2.8, no_inh_sigma, 0, -1.1, ...
  circularity, normalization));
sim.addElement(LateralInteractions1D('orl -> ael', fieldSize, 3, 1.5, no_inh_sigma, 0, 0.0, ...
  circularity, normalization));
sim.addElement(NormalNoise('normal noise ael', fieldSize, noise_amplitude));

sim.addConnection('ael', 'output', 'ael -> ael');
sim.addConnection('ael -> ael', 'output', 'ael');
sim.addConnection('normal noise ael', 'output', 'ael');
sim.addConnection('asl', 'output', 'asl -> ael');
sim.addConnection('asl -> ael', 'output', 'ael');
sim.addConnection('orl -> ael', 'output', 'ael');
sim.addConnection('orl', 'output', 'orl -> ael');


%% setting up the GUI

elementGroups = {'aol', 'asl', 'orl', 'ael', ...
  'hand position stimulus', 'object stimulus 1', 'object stimulus 2', 'object stimulus 3', ...
  'aol -> aol', 'asl -> asl', 'orl -> orl', 'ael -> ael', ...
  'aol -> asl', 'orl -> asl', 'asl -> ael', 'orl -> ael', ...
  'normal noise aol', 'normal noise asl', 'normal noise orl', 'normal noise ael'};

gui = StandardGUI(sim, [50, 50, 1200, 860], 0.01, [0.0, 0.3, 1.0, 0.7], [2, 2], 0.08, ...
  [0.0, 0.0, 1.0, 0.3], [4, 4], elementGroups, elementGroups, 'ICSR VR HRI');

xAxis = (1 : fieldSize) * d_x;
plotAxes = {'YLim', [-10, 15], 'XLim', [d_x, x_max], 'XGrid', 'on', 'YGrid', 'on'};
activationStyle = {'b', 'LineWidth', 3, 'XData', xAxis};
outputStyle = {'r', 'LineWidth', 2, 'XData', xAxis};
stimulusStyle = {'Color', [0, 0.75, 0], 'LineWidth', 2, 'XData', xAxis};

gui.addVisualization(MultiPlot({'aol', 'aol', 'hand position stimulus'}, {'activation', 'output', 'output'}, ...
  [1, 10, 1], 'horizontal', plotAxes, {activationStyle, outputStyle, stimulusStyle}, ...
  'aol - action observation (blue: activation, red: 10x output, green: hand stimulus)', ...
  'position', 'activation'), [1, 1]);
gui.addVisualization(MultiPlot({'orl', 'orl', 'object stimulus 1', 'object stimulus 2', 'object stimulus 3'}, ...
  {'activation', 'output', 'output', 'output', 'output'}, [1, 10, 1, 1, 1], 'horizontal', ...
  plotAxes, {activationStyle, outputStyle, stimulusStyle, stimulusStyle, stimulusStyle}, ...
  'orl - object memory (blue: activation, red: 10x output, green: object stimuli)', ...
  'position', 'activation'), [1, 2]);
gui.addVisualization(MultiPlot({'asl', 'asl'}, {'activation', 'output'}, [1, 10], 'horizontal', ...
  plotAxes, {activationStyle, outputStyle}, ...
  'asl - action simulation (blue: activation, red: 10x output)', 'position', 'activation'), [2, 1]);
gui.addVisualization(MultiPlot({'ael', 'ael'}, {'activation', 'output'}, [1, 10], 'horizontal', ...
  plotAxes, {activationStyle, outputStyle}, ...
  'ael - action execution (blue: activation, red: 10x output)', 'position', 'activation'), [2, 2]);

% stimulus sliders: positions are shown in dnf-composer units (scaling factor d_x)
% the hand stimulus is normalized (as in the original), so it needs a much larger amplitude
gui.addControl(ParameterSlider('p_hand', 'hand position stimulus', 'position', [0, x_max], '%0.1f', d_x, ...
  'position of the hand stimulus (dnf-composer units)'), [1, 1]);
gui.addControl(ParameterSlider('a_hand', 'hand position stimulus', 'amplitude', [0, 100], '%0.1f', 1, ...
  'amplitude of the hand stimulus (normalized Gaussian)'), [1, 2]);
gui.addControl(ParameterSlider('p_obj1', 'object stimulus 1', 'position', [0, x_max], '%0.1f', d_x, ...
  'position of object stimulus 1 (dnf-composer units)'), [2, 1]);
gui.addControl(ParameterSlider('a_obj1', 'object stimulus 1', 'amplitude', [0, 20], '%0.1f', 1, ...
  'amplitude of object stimulus 1'), [2, 2]);
gui.addControl(ParameterSlider('p_obj2', 'object stimulus 2', 'position', [0, x_max], '%0.1f', d_x, ...
  'position of object stimulus 2 (dnf-composer units)'), [3, 1]);
gui.addControl(ParameterSlider('a_obj2', 'object stimulus 2', 'amplitude', [0, 20], '%0.1f', 1, ...
  'amplitude of object stimulus 2'), [3, 2]);
gui.addControl(ParameterSlider('p_obj3', 'object stimulus 3', 'position', [0, x_max], '%0.1f', d_x, ...
  'position of object stimulus 3 (dnf-composer units)'), [4, 1]);
gui.addControl(ParameterSlider('a_obj3', 'object stimulus 3', 'amplitude', [0, 20], '%0.1f', 1, ...
  'amplitude of object stimulus 3'), [4, 2]);

% buttons
gui.addControl(GlobalControlButton('Pause', gui, 'pauseSimulation', true, false, false, 'pause simulation'), [1, 3]);
gui.addControl(GlobalControlButton('Reset', gui, 'resetSimulation', true, false, true, 'reset simulation'), [2, 3]);
gui.addControl(GlobalControlButton('Parameters', gui, 'paramPanelRequest', true, false, false, 'open parameter panel'), [3, 3]);
gui.addControl(GlobalControlButton('Save', gui, 'saveParameters', true, false, true, 'save parameter settings'), [1, 4]);
gui.addControl(GlobalControlButton('Load', gui, 'loadParameters', true, false, true, 'load parameter settings'), [2, 4]);
gui.addControl(GlobalControlButton('Quit', gui, 'quitSimulation', true, false, false, 'quit simulation'), [3, 4]);


%% run the simulator in the GUI

gui.run(inf);
