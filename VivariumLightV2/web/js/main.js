var light_url = 'http://192.168.100.49/lightlevel';

function turnLightsOn() {
	console.log("Turning lighs on.");
	
	var data = new FormData();
	data.append("cool", 0);
	data.append("warm", 0);

	console.log(data);
	httpGetAsync(light_url, resp_callback, data);
}

function turnLightsOff() {

}

function resp_callback(resp) {
	console.log("Request received by client.")
	console.log(" text: " + resp);
}

function httpGetAsync(theUrl, callback, data)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            callback(xmlHttp.responseText);
    }
    xmlHttp.open("GET", theUrl, true); // true for asynchronous 
    xmlHttp.send(data);
}

function setup() {
	const checkbox = document.getElementById('lightsOn');


	checkbox.addEventListener('change', (event) => {
	  if (event.target.checked) {
	    turnLightsOn();
	  } else {
	  	
	  }
	})
};

window.onload = setup;