# BnoMaster

The BnoMaster is a c++ library written to run on the sparkfun esp32 thing, and connect to a Bosch BNO055 inertial measurement unit. Each operating mode of the BNO055 is available on the fly, as well as each power mode. The library then connects to a REST server to submit readings using CSV format.

## Getting Started

To get started, visit the espressif website below, and click the link "Get Started" to install the espressif esp-idf.

```
https://docs.espressif.com/projects/esp-idf/en/latest/
```

### Prerequisites

C/C++
GCC
Python


### Installing

Once the esp-idf has been installed, continue with cloning the BnoMaster repository to begin building and deploying.

```
cd esp-idf/
mkdir BnoMaster
git clone https://github.com/jmeckst/BnoMaster
```
## Code Documentation

/* ------------------------------------------------------------------------- */
module map

/* ------------------------------------------------------------------------- */
includes.h

/* ------------------------------------------------------------------------- */
defines.h

/* ------------------------------------------------------------------------- */
templates.h

/* ------------------------------------------------------------------------- */
rest

/* ------------------------------------------------------------------------- */
event

/* ------------------------------------------------------------------------- */
sparkfun

/* ------------------------------------------------------------------------- */
bno

/* ------------------------------------------------------------------------- */
main

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc
