/*jslint browser: true, devel: true, sloppy: true, vars: true, indent: 2 */

var base_url = 'http://thegrebs.com/~michael/.thermostat.pl?token=';

function fetchData(setpoint) {
  var response;
  var url = setpoint ? base_url : base_url + "&set=" + setpoint;

  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response.temp) {
          Pebble.sendAppMessage(response);
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  };
  req.send();
}

Pebble.addEventListener("ready",
  function () {
    console.log("ready");
    base_url = base_url + Pebble.getAccountToken();
  });

Pebble.addEventListener("appmessage",
  function (e) {
    if (e.payload.set) {
      console.log("Received set message: " + e.payload.set);
      fetchData(e.payload.set);
    } else {
      console.log("Received fetch message");
      fetchData();
    }
  });
