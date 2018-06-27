# Weather Systems
Weather system using NCLGL library (an OpenGL library). [Executable](https://github.com/ConnorLee2/WeatherSystemsGL/tree/master/Other/Executable) available in the Other folder. The aim of this was to create a weather system which affects an environment.

## Screenshots
### Rain weather
![alt text](https://raw.githubusercontent.com/ConnorLee2/WeatherSystemsGL/master/Other/rain.png "rain")
### Snow weather
![alt text](https://raw.githubusercontent.com/ConnorLee2/WeatherSystemsGL/master/Other/snow.png "snow")
### Day/night system
![alt text](https://raw.githubusercontent.com/ConnorLee2/WeatherSystemsGL/master/Other/daynight.png "day/night system")

## Features
* Rain/snow weather, made with a particle system.
* 3D environment, made with a heightmap.
* Day/night system, made with sky boxes.
* Wind simulation, by modifying the velocity of particles.
* Rain making ground "wet", done with slope based texturing and a blend factor.
* Snow making ground "snowy", done with slope based texturing and a blend factor.
* Snow accumulation on the ground, done by pulling the vertices of the heightmap but only those with a low slope value.

## Usage
* 1 - Display snow weather.
* 2 - Display rain weather.
* 3 - Remove weather simulation.
* 6 - Increase wind speed in the x direction.
* 7 - Decrease wind speed in the x direction.
* 8 - Increase wind speed in the z direction.
* 9 - Decrease wind speed in the z direction.
* 0 - Reset wind speed to 0.

## Credits
* [NCLGL](https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/) - an OpenGL library.
