import os
import webbrowser
import click as cli
import numpy as np
import plotly.graph_objs as go
from plotly.offline import download_plotlyjs, init_notebook_mode, plot, iplot
from helpers.data_loading import load_folder, load_sections
from helpers.signal_utils import times_from_feature_rate, pulse_for_section
from helpers.tempogram_utils import subtract_mean_clean


@cli.command()
@cli.argument('data_path')
@cli.option('--multiples', default=[1, 2, 4], multiple=True)
def main(data_path, multiples):
    if not os.path.exists(data_path):
        cli.echo(f'Directory {data_path} not found', err=False)
        return

    # Data preprocess
    d = load_folder(data_path)
    sections = load_sections(f'{data_path}/sections.txt')

    # Plot
    tempogram_plot = plot_tempogram(d, multiples, data_path)
    cyclic_tempogram_plot = plot_cyclic_tempogram(d, data_path)
    smooth_tempogram_plot = plot_smooth_tempogram(d, data_path)
    novelty_curve_plot = plot_novelty_curve(d, sections, multiples, data_path)
    overlap_curve_plot = plot_overlap_curve(d, sections, multiples, data_path)

    # Main plot write
    plot_height = 600
    main_plot_file = f'{data_path}/index.html'
    main_plot = open(main_plot_file, 'w')
    main_plot.write("<html><head></head><body>\n")
    main_plot.write(f'<object data="{os.path.basename(tempogram_plot)}" width="100%" height="{plot_height}"></object>')
    main_plot.write(
        f'<object data="{os.path.basename(cyclic_tempogram_plot)}" width="100%" height="{plot_height}"></object>')
    main_plot.write(
        f'<object data="{os.path.basename(smooth_tempogram_plot)}" width="100%" height="{plot_height}"></object>')
    main_plot.write(
        f'<object data="{os.path.basename(novelty_curve_plot)}" width="100%" height="{plot_height}"></object>')
    main_plot.write(
        f'<object data="{os.path.basename(overlap_curve_plot)}" width="100%" height="{plot_height}"></object>')
    main_plot.write("</body></html>")
    main_plot.close()
    webbrowser.open('file://' + os.path.realpath(main_plot_file))


def plot_tempogram(data, tempo_multiples, save_path, name='tempogram'):
    ref_tempo = data.tempogram_cyclic.parameters['ref_tempo']
    tempogram_clean_data = subtract_mean_clean(np.abs(data.tempogram.data))

    layout = go.Layout(title='Tempogram', showlegend=True,
                       xaxis=dict(title='Time (s)'), yaxis=dict(title='Tempo (BPM)'))
    fig = go.Figure(data=[], layout=layout)

    fig.add_heatmap(z=tempogram_clean_data, x=data.t.data, y=data.bpm.data, colorscale='Viridis', name='tempogram',
                    showscale=False)
    for multiple in tempo_multiples:
        label = f'Tempo {multiple}x' if multiple != 1 else 'Tempo ref'
        fig.add_scatter(x=data.t_smooth.data, y=data.tempo_curve.data * ref_tempo * multiple, name=label)

    filename = f'{save_path}/{name}.html'
    plot(fig, filename=filename, auto_open=False)
    return filename


def plot_cyclic_tempogram(data, save_path, name='cyclic_tempogram'):
    ref_tempo = data.tempogram_cyclic.parameters['ref_tempo']
    tempogram_cyclic_clean_data = subtract_mean_clean(np.abs(data.tempogram_cyclic.data))

    layout = go.Layout(title=f'Cyclic Tempogram | Ref tempo = {ref_tempo}', showlegend=True,
                       xaxis=dict(title='Time (s)'), yaxis=dict(title='Tempo (rBPM)'))
    fig = go.Figure(data=[], layout=layout)

    fig.add_heatmap(z=tempogram_cyclic_clean_data, x=data.t.data, y=data.ct_y_axis.data, colorscale='Viridis',
                    showscale=False, name='tempogram')
    fig.add_scatter(x=data.t_smooth.data, y=data.tempo_curve.data, name='peak tempo')

    filename = f'{save_path}/{name}.html'
    plot(fig, filename=filename, auto_open=False)
    return filename


def plot_smooth_tempogram(data, save_path, name='smooth_tempogram'):
    smooth_length = data.smooth_tempogram.parameters['smooth_length']

    layout = go.Layout(title=f'Smoothed Tempogram | Temp Units = {smooth_length}', showlegend=True,
                       xaxis=dict(range=[data.t.data[0], data.t.data[-1]], autorange=False, title='Time (s)'),
                       yaxis=dict(title='Tempo (rBPM)'))
    fig = go.Figure(data=[], layout=layout)
    fig.add_heatmap(z=data.smooth_tempogram.data, x=data.t_smooth.data, y=data.ct_y_axis.data, colorscale='Viridis',
                    name='tempogram', showscale=False)
    fig.add_scatter(x=data.t_smooth.data, y=data.tempo_curve.data, name='peak tempo')

    filename = f'{save_path}/{name}.html'
    plot(fig, filename=filename, auto_open=False)
    return filename


def plot_novelty_curve(data, sections, multiples, save_path, name='novelty_curve'):
    t_nc = times_from_feature_rate(data.novelty_curve.parameters['feature_rate'], 0, len(data.novelty_curve.data))
    feature_rate = data.novelty_curve.parameters['feature_rate']

    pulses = [np.zeros_like(data.novelty_curve.data) for i in range(len(multiples))]
    for i in range(len(multiples)):
        for section in sections:
            cosine, _ = pulse_for_section(section, feature_rate, multiple=multiples[i])
            start = int(section.start * feature_rate)
            pulses[i][start:start + len(cosine)] = cosine

    mag = max(data.novelty_curve.data) * 0.2

    layout = go.Layout(title=f'Novelty curve & Computed BPM Onsets', showlegend=True,
                       xaxis=dict(title='Time (s)'), yaxis=dict(title='Amplitude'))
    fig = go.Figure(data=[], layout=layout)

    fig.add_scatter(x=t_nc, y=data.novelty_curve.data, name='novelty curve')
    for i in range(len(multiples)):
        y = pulses[i] * mag * (pulses[i] > 0)
        fig.add_scatter(x=t_nc, y=y, name=f'1/{multiples[i] * 4} notes')

    filename = f'{save_path}/{name}.html'
    plot(fig, filename=filename, auto_open=False)
    return filename


def plot_overlap_curve(data, sections, multiples, save_path, name='overlap_curve'):
    t_nc = times_from_feature_rate(data.novelty_curve.parameters['feature_rate'], 0, len(data.novelty_curve.data))
    feature_rate = data.novelty_curve.parameters['feature_rate']

    pulses = [np.zeros_like(data.novelty_curve.data) for i in range(len(multiples))]
    for i in range(len(multiples)):
        for section in sections:
            cosine, _ = pulse_for_section(section, feature_rate, multiple=multiples[i])
            start = int(section.start * feature_rate)
            pulses[i][start:start + len(cosine)] = cosine

    layout = go.Layout(title=f'Overlap Novelty curve & Computed BPM Onsets', showlegend=True,
                       xaxis=dict(title='Time (s)'), yaxis=dict(title='Amplitude'))
    fig = go.Figure(data=[], layout=layout)

    for i in range(len(multiples)):
        y = pulses[i] * data.novelty_curve.data
        fig.add_scatter(x=t_nc, y=y, name=f'1/{multiples[i] * 4} notes')

    filename = f'{save_path}/{name}.html'
    plot(fig, filename=filename, auto_open=False)
    return filename


if __name__ == '__main__':
    main()
