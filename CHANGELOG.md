# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
This changlog uses the [ISO 8601 date format](https://www.iso.org/iso-8601-date-and-time-format.html) of (YYYY-MM-DD).

## [Unreleased]

## [1.3.0] - 2021-01-22

### Added

* Added N64 Console support #29 (Thanks @JonnyHaystack)

## [1.2.1] - 2017-01-27

### Fixed

* Fix compiling of assembler functions

## 1.2 - 2016-07-19

### Added

* Added N64Controller support
* Added GamecubeConsole support
* Added library.properties
* Added version identifier

### Changed

* Reworked library structure
* Separated consoles

### Fixed

* Untabified all source files

## [1.1] - 2015-02-14

### Added

* Multiple controllers possible
* Added new begin() function overload

### Changed

* Hardcoded all get/send functions with assembler
 * Improved pin management/ram usage
 * Improved end() function
 * Improved example
 * Updated HID Project example to version 2.1
 * Updated structs
* Moved documentation to the Wiki

### Fixed

* Now with all IDE versions compatible

## [1.0] - 2014-07-01

### Added
* Added Gamecube Device support


[Unreleased]: https://github.com/NicoHood/Nintendo/compare/1.3.0...HEAD
[1.3.0]: https://github.com/NicoHood/Nintendo/compare/1.2.1...1.3.0
[1.2.1]: https://github.com/NicoHood/Nintendo/releases/tag/1.2.1
[1.1]: https://github.com/NicoHood/Nintendo/compare/1.0...1.1
[1.0]: https://github.com/NicoHood/Nintendo/releases/tag/1.0
