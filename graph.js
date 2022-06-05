//Define Global Variables
var count = 0; //Count of datapoints graphed
var timer; //Interval for plotting data
var circumference = 0; //Circumference of the wheel
var current_revolutions=0; //Shows number of revolutions done
var current_heartrate=0; //Shows the current heart rate
var current_temp = 0; //Current temperature
var mass; //Mass of rider
var total_heartrate = 0; //used for calculating average heartrate
var calories_burned = 0; //Tracks total calories burned
var met = 0; //Value of MET for calculating calories burned in a second
var mph = 0; //Current miles per hour


function createSpeedGraph(){ //This function initially creates the speed graph
    var trace1 = {
        type: 'lines + markers',
        y: [0],
        marker: {
            color: '#9403FC',
            line: {
                width: 2.5
            }
        }
      };
    
      var layout = { 
        title: 'Speed Graph',
        font: {size: 18},
        xaxis: {
            title: 'Time (s)',
            titlefont: {
            family: 'Arial, sans-serif',
            size: 16,
            color: 'lightgrey'
            }
        },
        yaxis: {
            title: 'Speed (MPH)',
            titlefont: {
            family: 'Arial, sans-serif',
            size: 16,
            color: 'lightgrey'
            }
        }
        /*,
        width: 500,
        height: 500,
        margin: {
          l: 50,
          r: 50,
          b: 100,
          t: 100,
          pad: 4
        }*/
      };
      
      var config = {responsive: true, scrollZoom: true};
    
    
    Plotly.newPlot('chart', [trace1], layout, config)
}

function createHeartGraph(){ //This function initially creates the heart graph
    var trace1 = {
        type: 'lines + markers',
        y: [0],
        marker: {
            color: '#FF0000',
            line: {
                width: 2.5
            }
        }
      };
    
      var layout = { 
        title: 'Heart Rate Monitor',
        font: {size: 18},
        xaxis: {
            title: 'Time (s)',
            titlefont: {
            family: 'Arial, sans-serif',
            size: 16,
            color: 'lightgrey'
            }
        },
        yaxis: {
            title: 'Heart Rate (bpm)',
            titlefont: {
            family: 'Arial, sans-serif',
            size: 16,
            color: 'lightgrey'
            }
        }
      };
      
      var config = {responsive: true, scrollZoom: true};
    
    
    Plotly.newPlot('heartbeat', [trace1], layout, config)
}

function setRadius(){ //Function sets wheel radius when button clicked
    circumference = parseInt(document.getElementById("radius").value) * Math.PI * 2;
    console.log(circumference);
}

function setMass(){ //Function sets user mass when button clicked
    mass = parseInt(document.getElementById("mass").value);
    console.log(mass);
}

function getData(){ //This function gets data from JSON packets
    const url = 'url of get request'
    //current_heartrate = Math.random();
    fetch(url)
    .then(response => response.json())  
    .then(json => {
        current_revolutions=json.speed
        current_heartrate=json.heartRate
       current_temp = json.temperature
        document.getElementById("json").innerHTML = JSON.stringify(json)
        
    })
 
    console.log(current_revolutions);
    console.log(current_heartrate);

}

class SpeedQueue{
    constructor(){
        this.samples = [];
        this.distance = 0;
    }

    enqueue(reading){ //Adds a value to the queue and calculates the distance of it
        this.distance = this.distance + (reading * circumference);
        if(this.samples.length == 10){
            this.samples.shift();
        }
        this.samples.push(reading * circumference);
        console.log(this.samples);
    }

    read(){ //Reads the current mph by calculating average of a sample
        var total = 0;
        for (var i = 0; i < this.samples.length; i++) { //10 second sample for speed
            total = this.samples[i] + total;
        }
        return ((total/this.samples.length) * 0.0223694); //Conversion from cm/s to MPH
    }

    get_distance(){//Gets the total distance travelled
        return this.distance * 0.0000062137;
    }

    get_average(){ //Gets the average speed across trip
        return ((this.distance/count) * 0.0223694);
    }


}

function startPlotting(){ //This function starts the interval for plotting data
    if(timer == null){ //Make sure only one interval is set up at a time
        var speed = new SpeedQueue();
        timer = setInterval(function(){
            getData();
            speed.enqueue(current_revolutions);
            count++;
            if(count > 60){ //Shifts graph to show only a minute range
                Plotly.relayout('chart', {xaxis: {range: [count-60, count]}});
                Plotly.relayout('heartbeat', {xaxis: {range: [count-60, count]}});
            }
            //Updates graph datapoints
            mph = speed.read();
            Plotly.extendTraces('chart',{y:[[mph]]}, [0]);
            Plotly.extendTraces('heartbeat',{y:[[current_heartrate]]}, [0]);
            total_heartrate = total_heartrate + current_heartrate;
            if(mph > 20){
                met = 15.8;
            }
            else if (mph > 16){
                met = 12;
            }
            else if (mph > 14){
                met = 10;
            }
            else if (mph > 12){
                met = 8;
            }
            else if (mph > 10){
                met = 6.8;
            }
            else if (mph > 5.5){
                met = 4;
            }
            else {
                met = 0;
            }
            calories_burned = calories_burned + ((3.5 /60 *  mass * met)/200);
            document.getElementById("p").innerHTML = "Average Heart Rate: " + total_heartrate/count + " BPM";
            document.getElementById("p1").innerHTML = "Total Distance: " + speed.get_distance() + " Miles";
            document.getElementById("p2").innerHTML = "Average Speed: " + speed.get_average() + " MPH";
            document.getElementById("p3").innerHTML = "Calories Burned: " + calories_burned + " KCal";
            document.getElementById("p4").innerHTML = "Temperature: " + current_temp + " Fahrenheit";

        }, 1000)
    }
}

function stopPlotting(){ //This function stops the interval for plotting data
    clearInterval(timer);
    timer = null;
}

createSpeedGraph();
createHeartGraph();
