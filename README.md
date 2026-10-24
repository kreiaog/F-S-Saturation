# F-S Saturation

F-S Saturation (Fast-Simple Saturation) - a lightweight, real-time safe saturation audio plugin.

## Description

### About

This project was started as a part of self-study "Audio Programming". F-S Saturation is a lightweight, real-time safe saturation audio plugin built with C++20 and JUCE. Features asymmetric waveshaping for warm harmonic generation, an adaptive DC-blocking filter, and automatic gain compensation.

### Key Features

* Assymetric Waveshaping.
* Adaptive DC-Blocker.
* Real-Time Safe Architecture.
* Automatic Gain Compensation.

## Getting Started

### Dependencies

* C++ 20
* JUCE 7.0.12

### Installing

* Clone the repository using: 
```
git clone https://github.com/kreiaog/F-S-Saturation.git
```
* Open the project's folder 
in your IDE.
* In terminal execute these two commands:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Authors

Vasyl Havryliuk - [@kreiaog](https://www.instagram.com/kreiaog/)

## Version History

* 0.1
    * The project created and added core functionality.
* 0.2
    * Release version. Fixed bugs and optimized
* 0.3
    * Added Oversampling

See [commit change]() or See [release history]().

## License

This project is licensed under the MIT License - see the LICENSE.md file for details