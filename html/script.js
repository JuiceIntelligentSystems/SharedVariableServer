var variable = document.getElementById("variable"),
    variable_val = document.getElementById("variable-val");

variable_val.innerHTML = variable.value;

variable.oninput = function () {
    var data = JSON.stringify({ variableValue: this.value });
    variable_val.innerHTML = this.value;

    // Options
    var requestOptions = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: data,
    };

    // Make HTTP Request
    fetch('http://localhost:8080', requestOptions)
        .then(response => {
            if (response.status === 200) {
                return response.json(); // Parse JSON when status is 200 OK
            }
            else if (response.status === 204) {
                // console.log("No Content");
            }
            else {
                // Handle Error if no Response
                throw new Error("Response Error");
            }
        })
        /*
        .then(data => {
            // Process Response Data
            console.log(data);
        })
        */
        .catch(error => {
            // Handle More Errors
            console.error("Error: ", error);
        });
}

function getData() {
    // Send GET Request
    var requestOptions = {
        method: 'GET',
    };
    fetch('http://localhost:8080/data', requestOptions)
        .then(response => {
            if (!response.ok) {
                throw new Error("Request Failed");
            }
            return response.text();
        })
        .then(data => {
            // Process Data Received Here
            var object = JSON.parse(data);
            document.getElementById("randomNumber").innerHTML = object.number;
            // console.log(data);
        })
        .catch(error => {
            console.error('Error: ', error);
        });
}

setInterval(getData, 1000);
getData();