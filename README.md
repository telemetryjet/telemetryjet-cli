[![asciicast](https://asciinema.org/a/377404.svg)](https://asciinema.org/a/377404)

# TelemetryJet CLI

## Overview

The TelemetryJet CLI, `jet`, is a command-line tool for defining, monitoring, and controlling embedded telemetry systems.
It's part of the TelemetryJet platform, which makes it easier to collect, analyze and share data from hardware systems.

Use jet to stream data between a number of data sources, including serial devices, files, and the TelemetryJet server. 
Jet uses declarative configuration: You specify your data sources in JSON configuration files, and jet handles the 
details of streaming data between the sources.


## Use Cases

Jet is an extremely versatile tool. Here are some examples of how you can use it:

**Stream data to a CSV file:** Use jet to stream hardware telemetry data straight into a CSV file, without any programming required.

**Stream data to a TelemetryJet Server:** Stream data to a TelemetryJet Server, and build live dashboards and visualizations that are viewable from anywhere.

**Network multiple microcontrollers:** Use a Raspberry Pi as a central telemetry hub, building a network of microcontrollers which can communicate with each other bidirectionally.

**Monitor existing embedded platforms:** Subscribe to data from ROS or other standard platforms and stream into a TelemetryJet Server.

**Set up declarative configuration for embedded devices:** Define throttle curves, PID control terms, or other parameters centrally, and distribute new values without requiring code changes.

# Installation

Jet is available on a variety of Linux platforms, macOS and Windows 10. 
The CLI is distributed using platform-specific methods; see the platform-specific pages on the [TelemetryJet Documentation](https://docs.telemetryjet.com/cli/)
for individual instructions.

The CLI binaries are also available as static files for manual installation from the [TelemetryJet Download Server](https://downloads.telemetryjet.com/builds/cli/).

# Usage
Jet uses declarative configuration: You specify the data source settings in a JSON file, and jet builds a network that connects all data sources in a file together.

## Basic Usage

To stream data, use the `jet stream` command. The command takes one or more JSON files as inputs:
```
jet stream device.json # One configuration

jet stream device1.json device2.json # Multiple configurations

jet stream devices/*.json # All JSON files in a folder
```

You can specify JSON files with relative paths, absolute paths, or [Glob syntax](https://en.wikipedia.org/wiki/Glob_(programming)) to select files using a pattern. 

Jet is completely stateless -- All the configuration is set using the files and parameters you pass to the program. This means you can put your configuration in a git repository, flash drive, or other sharing mechanism.

## Configuration Files

A configuration file defines a network of multiple data sources. All the data sources in a file can communicate with each other bidirectionally. 
Some data sources only transmit data, others only receive data, and many do both. For more details about which data sources transmit and receive data, see the [Data Source Types](https://docs.telemetryjet.com/cli/guides/data_sources/) on the TelemetryJet Documentation site.

A configuration file contains an array of data source definitions:

```
[
  {
    "id": "arduino1",
    "type": "&lt;data source type&gt;",
    "options": {
      &lt;options for the data source&gt;
    }
  },
  {
    "id": "arduino2",
    "type": "&lt;data source type&gt;",
    "options": {
      &lt;options for the data source&gt;
    }
  }
]
```

# Supported Data Source Types

Jet can interface with many data source types, including files, serial devices, and software protocols. 

<table>
  <thead>
    <tr>
      <th style="width: 100px;">Type</th>
      <th style="width: 100px;">Read/Write/Both</th>
      <th style="width: 200px;">Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="https://docs.telemetryjet.com/cli/guides/data_sources/console">console</a></td>
      <td>Write</td>
      <td>Write data to the console output.</td>
    </tr>
    <tr>
      <td><a href="https://docs.telemetryjet.com/cli/guides/data_sources/key-value-file">key-value-file</a></td>
      <td>Write</td>
      <td>Write data into a text file in Key-Value (<code>key=value</code>) format.</td>
    </tr>
    <tr>
      <td><a href="https://docs.telemetryjet.com/cli/guides/data_sources/key-value-serial">key-value-serial</a></td>
      <td>Both</td>
      <td>Read and write data as Key-Value lines over a serial stream.</td>
    </tr>
  </tbody>
</table>

See the [Data Source Types](https://docs.telemetryjet.com/cli/guides/data_sources/) for a full list and individual guides.

This list will expand over time. We're currently working on core data source types, including CSV files and streams. Need a data source type that is not listed? [Open an Issue](https://github.com/telemetryjet/telemetryjet-cli/issues/new) on the CLI Github suggesting it should be officially supported.