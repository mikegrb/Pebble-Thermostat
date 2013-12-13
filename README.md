Pebble-Thermostat
=================

Control Thermostats with an API from Pebble

Needs an intermediary script available via HTTP.  The URL goes in pebble-js-app.js.
Script should output a JSON structure with 3 keys:

```{"temp":"68.5","mode":"Heat","set":"70"}```

When setpoint is changed from the watch set=$N is passed in as a URL parameter.

Displays mode, inside temp, setpoint with support for changing setpoint.

May add setting URL for control script via Pebble iOS/Android app configuration in the future if local config html is added.

![Screenshot](/screenshot/00036e4_annotated.png "Screenshot at 88901ca")
