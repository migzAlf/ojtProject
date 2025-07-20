#include <WiFi.h>
#include <WebServer.h>

// AP Credentials
#define ssid "2AxisController"
#define password "control123"

#pragma region L298_input_pins
#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25

#define IN5 15
#define IN6 2
#define IN7 0
#define IN8 4
#pragma endregion

#pragma region Joystick_pins
// Joystick setup
#define JoystickX 33
#define JoystickY 32
#pragma endregion 

#pragma region motorSetup
const int nema17MotorSteps = 200; 
int xVal, yVal;
const int stepSequence[4][4] = {
  {1, 0, 1, 0},  // Step 1
  {0, 1, 1, 0},  // Step 2
  {0, 1, 0, 1},  // Step 3
  {1, 0, 0, 1}   // Step 4
};
const int reverseStepSequence[4][4] = {
  {1, 0, 0, 1},  // Step 4
  {0, 1, 0, 1},  // Step 3
  {0, 1, 1, 0},  // Step 2
  {1, 0, 1, 0}   // Step 1
};
int posX = 0;
int posY = 0;

int stepIndex = 0;
unsigned long lastStepTime = 0;
const int stepDelay = 2.5;  // milliseconds between steps (adjust to control speed)
#pragma endregion

#pragma region AP_Setup
WebServer server(80);
#pragma endregion

#pragma region Set_Up_Pin_modes
void setPinModes () {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);
}
#pragma endregion

#pragma region WebServer_region
void handleRoot () {
  server.send(200, "text/html", 
  R"rawliteral(
        <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>2 Axis Controller</title>
        <style>
            body {
                margin: 0;
                font-family: sans-serif;
            }

            .navBarContainer {
                height: 100px;
                width: 700px;
                border-bottom: 1px solid #E4E4E4;
                display: flex;
                align-items: center;
            }
            .navBarTitle {
                font-size: 2rem;
                font-weight: 1000;
                width: 750px;
                height: 100%;
                display: flex;
                align-items: center;
                padding-left: 5rem;
            }
            .navBarTitle2 {
                font-size: 1.3rem;
                font-weight: bolder;
                height: 100%;
                width: 200px;
                display: flex;
                justify-content: end;
                align-items: center;
                padding-right: 5rem;
            }

            .canvasWrapper {
                width: 170%;
                height: 700px;
                display: flex;
                justify-content: center;
                align-items: center;
            }
            .mainCanvas {
                border: 1px solid black;
                transform: rotateZ(180deg) scale(0.7);
            }
            .controllersWrapper {
                width: 700px;
                height: 60vh;
            }
            .YAxisControllerWrapper {
                width: 100%;
                height: 50%;
                display: flex;
                justify-content: center;
                align-items: center;
            }
            .XAxisControllerWrapper {
                width: 100%;
                height: 50%;
                display: flex;
                justify-content: center;
                align-items: center;
            }

            .YAxisControllerContainer, .XAXisControllerContainer {
                width: 80%;
                height: 500px;
                outline: 1px solid #8E8E8E;
                border-radius: 16px;
                margin-top: 300px;
                display: flex;
                justify-content: center;
                align-items: center;
            }

            .XAXisControllerContainer {
                margin-top: 900px;
            }
            .controllerContentContainer {
                margin-top: -300px;
                height: 200px;
                width: 100%;
            }
            
            .controllerContentContainer1stRow {
                height: 200px;
                width: 100%;
                display: grid;
                grid-template-columns: 1fr 1fr;
            }

            .controllerContent2ndRow {
                height: 50%;
                width: 100%;
                display: grid;
                grid-template-columns: 1fr 1fr;
            }
          
            .controllerContainerElement1Wrapper, .controllerContainerElement2Wrapper, .controllerContainerElement3Wrapper {
                display: flex;
                justify-content: center;
                align-items: center;
                width: 250px;
                height: 300px;
            }
            .cce2Container {
                width: 300px;
                height: 150px;
                background-color: blue;
                color: white;
                outline: 1px solid black;
                display: flex;
                justify-content: center;
                align-items: center;
                border-radius: 40px;
                flex-direction: column;
            }
            .cce2ContainerY {
                width: 300px;
                height: 150px;
                background-color: red;
                color: white;
                outline: 1px solid black;
                display: flex;
                justify-content: center;
                align-items: center;
                border-radius: 40px;
                flex-direction: column;
            }
            .cce1Container {
                width: 80%;
                height: 50%;
                outline: 1px solid #E4E4E4;
                border-radius: 16px;
                display: flex;
                justify-content: center;
                align-items: center;
                flex-direction: column;
                box-shadow: -1px 7px 4px 1px rgba(0,0,0,0.16);

            }
            .cce1Button {
                width: 80%;
                height: 35%;
                outline: 1px solid #8E8E8E;
                border-radius: 16px;
                display: flex;
                justify-content: center;
                align-items: center;
                font-size: 2rem;
                font-weight: bold;
                box-shadow: -1px 7px 4px 1px rgba(0,0,0,0.16);
            }

            .cce3Container {
                width: 600px;
                height: 200px;
                outline: 1px solid #8E8E8E;
                box-shadow: -1px 7px 4px 1px rgba(0,0,0,0.16);
                border-radius: 16px;
            }
            .cce3Container > h1 {
                width: 100%;
                text-align: center;
            }
            .cce3ControllerContainer {
                display: flex;
                justify-content: center;
                align-items: center;
                width: 100%;
                height: 40%;
            }
            .cce3button {
                width: 30%;
                margin: 1.5rem;
                height: 90%;
                outline: 1px solid black;
                border-radius: 16px;
                display: flex;
                justify-content: center;
                align-items: center;
            }
            .cce3button:active {
                background-color: red;
            }
        </style>
        
        <!-- Prompt style -->
        <style>
            .shadowBackground {
                width: 700px;
                height: 2000px;
                display: block;
                position: absolute;
                background-color: black;
                z-index: 10;
                opacity: .75;
            }
            .promptWrapper {
                width: 700px;
                height: 1250px;
                display: flex;
                justify-content: center;
                align-items: center;
                position: absolute;
                z-index: 11;
                position: absolute;

            }
            .promptContainer {
                width: 800px;
                height: 600px;
                background-color: white;
                border-radius: 16px;
                position: absolute;
                z-index: 30;
                display: flex;
                justify-content: center;
                align-items: center;
                flex-direction: column;
                transform: scale(.7);
            }

            .promptContainer > h1 {
                font-size: 3rem;
            }
            .promptText {
                width: 80%;
                height: 100px;
                font-size: 3rem;
            }
            .promptInputWrapper {
                width: 100%;
                height: auto;
                display: flex;
                justify-content: center;
                align-items: center;
                flex-direction: column;
            }
            .promptInputButtonsContainer {
                display: flex;
                justify-content: center;
                align-items: center;
                width: 100%;
                height: auto;
            }
            .promptInputButtonsContainer > * {
                margin: 50px;
                width: 30%;
                height: 75px;
                outline: 1px solid black;
                border-radius: 16px;
                display: flex;
                justify-content: center;
                align-items: center;
                font-size: 2rem;
                font-weight: bold;
            }

            .promptInputButtonsContainer > *:active {
                background-color: black;
                color: white;
            }

            .promptSetButton {
                background-color: blue;
                color: white;
            }

            #promptDivsContainer {
                display: none;
            }
        </style>
    </head>
    <body>

        <!-- PROMPT DIVS -->
        <div id="promptDivsContainer">
            <div class="promptWrapper">
                <div class="promptContainer">
                    <h1 id="promptTitle">SET RPM</h1>
                    <div class="promptInputWrapper">
                        <input id="promptText" class="promptText" type="text">
                        <h2 id="inputNumberWarning">Please Enter a Positive Number</h2>
                        <div class="promptInputButtonsContainer">
                            <div id="promptCancelButton" class="promptCancelButton">Cancel</div>
                            <div id="promptSetButton" class="promptSetButton">Set</div>
                        </div>
                    </div>
                </div>
            </div>
            <div id="shadowBackground" class="shadowBackground">
            </div>
        </div>
      

        <div class="navBarContainer">
            <h1 class="navBarTitle">2 - Axis Camera Controller</h1>
            <h1 class="navBarTitle2">ESP32-AP</h1>
        </div>
        <div class="canvasWrapper">
            <canvas id="mainCanvas" width="950" height="950" class="mainCanvas"></canvas>
        </div>
        <div class="controllersWrapper">
            <div class="YAxisControllerWrapper">
                <div class="YAxisControllerContainer">
                    <div class="controllerContentContainer">
                        <div class="controllerContentContainer1stRow">
                            <div class="controllerContainerElement1Wrapper">
                                <div class="cce1Container">
                                    <h1 id="rps1Text">RPS: </h1>
                                    <div id="sprSetButton" class="cce1Button">SET</div>
                                </div>
                            </div>
                            <div class="controllerContainerElement2Wrapper">
                                <div class="cce2Container">
                                    <h1>Motor 1</h1>
                                    <h1>Y Axis</h1>
                                </div>
                            </div>
                        </div>
                        <div class="controllerContent2ndRow">
                            <div class="controllerContainerElement1Wrapper">
                                <div class="cce1Container">
                                    <h1 id="rpm1Text">RPM: </h1>
                                    <div id="rpmSetButton" class="cce1Button">SET</div>
                                </div>
                            </div>
                            <div class="controllerContainerElement3Wrapper">
                                <div class="cce3Container">
                                    <h1>Movement</h1>
                                    <div class="cce3ControllerContainer">
                                        <div id="moveDownButton" class="cce3button">▼</div>
                                        <div id="moveUpButton" class="cce3button">▲</div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div class="XAxisControllerWrapper">
                <div class="XAXisControllerContainer">
                    <div class="controllerContentContainer">
                        <div class="controllerContentContainer1stRow">
                            <div class="controllerContainerElement1Wrapper">
                                <div class="cce1Container">
                                    <h1 id="rps2Text">RPS: </h1>
                                    <div id="sprSetButton2" class="cce1Button">SET</div>
                                </div>
                            </div>
                            <div class="controllerContainerElement2Wrapper">
                                <div class="cce2ContainerY">
                                    <h1>Motor 1</h1>
                                    <h1>X Axis</h1>
                                </div>
                            </div>
                        </div>
                        <div class="controllerContent2ndRow">
                            <div class="controllerContainerElement1Wrapper">
                                <div class="cce1Container">
                                    <h1 id="rpm2Text">RPM: </h1>
                                    <div id="rpmSetButton2" class="cce1Button">SET</div>
                                </div>
                            </div>
                            <div class="controllerContainerElement3Wrapper">
                                <div class="cce3Container">
                                    <h1>Movement</h1>
                                    <div class="cce3ControllerContainer">
                                        <div id="moveLeftButton" class="cce3button">◄</div>
                                        <div id="moveRightButton" class="cce3button">►</div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

        </div>

        
        <!-- SCRIPTS -->
        
        <!-- PROMPT SCRIPT -->
        <script>
            //debug
            const flag = false;


            // prompting buttons
            const [sprSetButton, rpmSetButton, sprSetButton2,rpmSetButton2] = [
                document.getElementById("sprSetButton"), 
                document.getElementById("rpmSetButton"), 
                document.getElementById("sprSetButton2"), 
                document.getElementById("rpmSetButton2")
            ]

            // prompt elements
            const promptDiv = document.getElementById("promptDivsContainer");
            const promptValue = document.getElementById("promptText");
            const promptTitle = document.getElementById("promptTitle");
                                    
            // property elements 
            const [rps1Text, rpm1Text, rps2Text, rpm2Text] = [
                document.getElementById("rps1Text"),
                document.getElementById("rpm1Text"),
                document.getElementById("rps2Text"),
                document.getElementById("rpm2Text")
            ]

            let setState = "rpm1"

            sprSetButton.addEventListener("click", () => {
                promptDiv.style.display = "block";
                promptTitle.textContent = "SET RPS Y-Axis"
                promptValue.value = "";
                setState = "rps1";
            })
            rpmSetButton.addEventListener("click", () => {
                promptDiv.style.display = "block";
                promptTitle.textContent = "SET RPM Y-Axis"
                promptValue.value = "";
                setState = "rpm1";
            })
            sprSetButton2.addEventListener("click", () => {
                promptDiv.style.display = "block";
                promptTitle.textContent = "SET RPS X-Axis"
                promptValue.value = "";
                setState = "rps2";
            })
            rpmSetButton2.addEventListener("click", () => {
                promptDiv.style.display = "block";
                promptTitle.textContent = "SET RPM X-Axis"
                promptValue.value = "";
                setState = "rpm2";
            })

            const warningLabel = document.getElementById("inputNumberWarning");
            document.getElementById("promptCancelButton").addEventListener("click", () => {
                promptDiv.style.display = "none";
                warningLabel.style.display = "none";
            })

            warningLabel.style.display = "none";
            warningLabel.style.color = "red";
            document.getElementById("promptSetButton").addEventListener("click", () => {
                //check if its a number
                const regex = /^(?:[1-9]\d*|0)?(?:\.\d+)?$/;
                if (!regex.test(promptValue.value)) {         
                    warningLabel.style.display = "block";
                    console.log("not a number")
                    return; 
                }
                
                
                warningLabel.style.display = "none";
                promptDiv.style.display = "none";
                switch(setState) {

                    case "rpm1":
                        fetch("/set?rpm1=" + promptValue.value).then(data => {
                            rpm1Text.textContent = data;
                        });
                        break;
                    case "rps1":
                        fetch("/set?rps1=" + promptValue.value).then(data => {
                            rps1Text.textContent = data;
                        });
                        break;
                    case "rpm2":
                        fetch("/set?rpm2=" + promptValue.value).then(data => {
                            rpm2Text.textContent = data;
                        });
                        break;
                    case "rps2":
                        fetch("/set?rps2=" + promptValue.value).then(data => {
                            rps2Text.textContent = data;
                        });
                        break;              
                }
            })

            // initialize values
            window.addEventListener("load", () => {
                if (flag) {    
                    fetch("/getRps1").then(data=> {
                        rps1Text.textContent = "RPS: " + parseInt(data);
                    })
                    fetch("/getRpm1").then(data=> {
                        rpm1Text.textContent = "RPM: " + parseInt(data);
                    })
                    fetch("/getRps2").then(data=> {
                        rps2Text.textContent = "RPS: " + parseInt(data);
                    })
                    fetch("/getRpm2").then(data=> {
                        rpm2Text.textContent = "RPM: " + parseInt(data);
                    })
                }
            })
        </script>

        <!-- CANVAS SCRIPT -->
        <script>
            const canvas = document.getElementById("mainCanvas");
            const ctx = canvas.getContext("2d");

            //CONFIG
            const woodBaseWidth = 700;
            const woodBaseHeight = 100;
            const YAxisAluminumWidth = 500;
            const YAxisAluminumHeight = 50;
            const YAxisCarriageWidth = 90;
            const YAxisCarriageHeight = 90;
            const XAxisCarriageWidth = 90;
            const XAxisCarriageHeight = 90;


            // For UI Control
            const elementCollection = [];
            const leftMoveButton = document.getElementById("leftMoveUI");
            const rightMoveButton = document.getElementById("rightMoveUI");
            const currentNameInput = document.getElementById("elementName");
            const speedUI = document.getElementById("speedUI");
            const distanceUI = document.getElementById("distanceUI");

            function findElement(name) {
                let elementTemp; 
                elementCollection.forEach(element => {
                    console.log(`comparing ${element.name} to ${name}: ${element.name == name}`)
                    if (element.name == name) { 
                        elementTemp = element;
                    }
                })
                return elementTemp;
            }


            function init() {
                window.requestAnimationFrame(draw);
            }

            let drawElements = [];

            function draw() {
                
                ctx.clearRect(0,0, 950,950);

                //draw wood base
                drawWoodBase();
                //draw the aluminum bars
                drawYAxisAluminumBar();

                //draw other stuff
                // drawElements.forEach(element => {
                //     element.draw();
                // })

                reddy.draw();


                window.requestAnimationFrame(draw);

            }


            class CarriagePorter {
                constructor([xPos, yPos, width, height, color], parentCarriageName, childCarriages) {
                    this.parentCarriage = new Carriage([xPos, yPos, width, height, color], parentCarriageName);
                    this.childCarriages = childCarriages;
                }

                draw () {
                    this.parentCarriage.draw();
                    this.childCarriages.forEach(child => {
                        child.draw();
                    })
                }

                setPosXwithOffset (value) {
                    this.parentCarriage.setPosXwithOffset(value);
                    this.childCarriages.forEach(child => {
                        child.setPosXwithOffset(value);
                    })
                }

                setPosYwithOffset (value) {
                    this.parentCarriage.setPosYwithOffset(value);
                    this.childCarriages.forEach(child => {
                        child.setPosYwithOffset(value);
                    })
                }

                moveX (value, speed) {
                    this.parentCarriage.moveX(value, speed);
                    this.childCarriages.forEach(child => {
                        child.moveX(value, speed);
                    })
                } 
                moveY (value, speed) {
                    this.parentCarriage.moveY(value, speed);
                    this.childCarriages.forEach(child => {
                        child.moveY(value, speed);
                    })
                }
            }
            class Carriage {
                constructor([xPos, yPos, width, height, color], name = "John Square") {
                    this.xPos = xPos;
                    this.yPos = yPos;
                    this.width = width;
                    this.height = height;
                    this.color = color;
                    this.name = name;

                    //For UI
                    elementCollection.push(this);

                    //Movement Controls
                    this.hasXMoveOrders = false;
                    this.hasYMoveOrders = false;
                    this.targetXPosition = 0;
                    this.targetYPosition = 0;       
                    this.moveXOrderSpeed = 0;
                    this.moveYOrderSpeed = 0;

                    //Movement Constraints
                    this.xOffset = this.xPos;
                    this.yOffset = this.yPos;
                    this.maxXMoveOrder = 460;
                    this.maxYMoveOrder = 300;

                    drawElements.push(this);
                }

                draw () {
                    ctx.fillStyle = this.color;
                    ctx.fillRect(this.xPos, this.yPos, this.width, this.height);

                    if (this.hasXMoveOrders) {
                        this.setXInMotion();
                    }
                    if (this.hasYMoveOrders) {
                        this.setYInMotion();
                    }
                }
                setPosX (value) {
                    this.xPos = value; 
                }

                setPosY (value) {
                    this.yPos = value;
                }

                setPosXwithOffset (value) {
                    this.xPos = value + this.xOffset;
                }

                setPosYwithOffset (value) {
                    this.yPos = value + this.yOffset;
                }

                moveX(value, speed) {
                    if ((this.xPos + value * (speed > 0 ? 1 : speed < 0 ? -1 : 0) > this.maxXMoveOrder + this.xOffset) || 
                    (this.xPos + value * (speed > 0 ? 1 : speed < 0 ? -1 : 0) < 0)) {
                        console.log("prevented x");
                        return;
                    }
                    this.hasXMoveOrders = true;
                    this.targetXPosition = value;
                    this.moveXOrderSpeed = speed;

                }
                moveY(value, speed) {
                    if ((this.yPos + value * (speed > 0 ? 1 : speed < 0 ? -1 : 0) > this.maxYMoveOrder + this.yOffset) || 
                    (this.yPos + value * (speed > 0 ? 1 : speed < 0 ? -1 : 0) < 0)) {
                        console.log("prevented Y");
                        return;
                    }
                    this.hasYMoveOrders = true;
                    this.targetYPosition = value;
                    this.moveYOrderSpeed = speed;
                }
                
                setXInMotion() {
                    if (this.targetXPosition > 0) {
                        this.setPosX(this.xPos + this.moveXOrderSpeed);
                        this.targetXPosition -= Math.abs(this.moveXOrderSpeed);
                        return;
                    }
                    this.hasXMoveOrders = false;
                }
                setYInMotion() {
                    if (this.targetYPosition > 0) {
                        this.setPosY(this.yPos + this.moveYOrderSpeed);
                        this.targetYPosition -= Math.abs(this.moveYOrderSpeed);
                        return;
                    }
                    this.hasYMoveOrders = false;
                }
            }

            function drawWoodBase() {
                ctx.fillStyle = "brown";
                ctx.fillRect(150
                    , 100, woodBaseWidth, woodBaseHeight);
            }

            //2020 Aluminum Extrusions
            function drawYAxisAluminumBar () {
                ctx.fillStyle = "black";
                ctx.fillRect(225, 125, YAxisAluminumWidth, YAxisAluminumHeight);
            }


            // Two Carriage and X axis Aluminum Bar
            // bluey - X axis or Camera Carriage
            // reddy - Y axis Carriage

            let bluey = new Carriage([205, 190, XAxisCarriageWidth, XAxisCarriageHeight, "blue"], "bluey");
            let xAxisAluminumBar = new Carriage([225,175, 50 ,381, "black"], "xAxisBar");
            let reddy = new CarriagePorter([205, 100, YAxisCarriageWidth, YAxisCarriageHeight, "red"], "reddy", [bluey, xAxisAluminumBar]);
            init();
        </script>

        <!-- API REQUESTS SCRIPT -->
        <script>
            // 1. Update simulation every .5s
            setInterval(()=>{
                if (flag) {
                    fetch("/getPos").then(response => response.json())
                    .then(data=> {
                        
                        let xPos = data.xPos;
                        let yPos = data.yPos;
                        
                        reddy.setPosXwithOffset(xPos);
                        bluey.setPosYwithOffset(yPos);
                        
                    })
                }
                    
            }, 500)
            // 2. set rpm
            function setRpm(value) {
                fetch("/set?rpm='" + value + "'");
            } 
            // 3. set rps
            function setRps(value) {
                fetch("/set?rps='" + value + "'");
            }
            
            //MOVEMENT
            const [moveRightButton, moveLeftButton, moveUpButton, moveDownButton] = [
                document.getElementById("moveRightButton"),
                document.getElementById("moveLeftButton"),
                document.getElementById("moveUpButton"),
                document.getElementById("moveDownButton")
            ]
            
            // 4. move right
            function moveRight() {
                fetch("/set?move='right'");
            }
            // 5. move left
            function moveLeft() {
                fetch("/set?move='left'");
            }
            // 6. move up 
            function moveUp() {
                fetch("/set?move='up'");
            }
            // 7. move down
            function moveDown() {
                fetch("/set?move='down'");
            }

            moveRightButton.addEventListener("click", moveRight);
            moveLeftButton.addEventListener("click", moveLeft);
            moveUpButton.addEventListener("click", moveUp);
            moveDownButton.addEventListener("click", moveDown);

            //#region Buttons 
            document.getElementById("sprSetButton").addEventListener("click", () => {
            })

            document.getElementById("rpmSetButton").addEventListener("click", () => {
                
            })

            document.getElementById("moveDownButton").addEventListener("click", () => {
                
            })

            document.getElementById("moveUpButton").addEventListener("click", () => {
                
            })

            document.getElementById("sprSetButton2").addEventListener("click", () => {
                
            })

            document.getElementById("rpmSetButton2").addEventListener("click", () => {
                
            })
            
            document.getElementById("moveLeftButton").addEventListener("click", () => {
                
            })

            document.getElementById("moveRightButton").addEventListener("click", () => {
                
            })
            //#endregion
        </script>
    </body>
    </html>
  )rawliteral");
}

void handleCommands () {
  argFunc("move", "right", []() {
    moveRight();
  });
  argFunc("move", "left", []() {
    moveLeft();
  });
  argFunc("move", "up", []() {
    moveUp();
  });
  argFunc("move", "down", []() {
    moveDown();
  });
}

void argFunc(String property, String value, void (*func)()) {
  if (server.hasArg(property)) {
    if (server.arg(property) == value) func();
  }
}

void setupAP () {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access point started ip: ");
  Serial.println(IP);
  
  server.on("/", handleRoot);
  server.on("/set", HTTP_GET, handleCommands);
  
  // Info requests


  server.begin();
  Serial.println("Web server Started.");
}
#pragma endregion

#pragma region motorMovementFunctions
void rotateMotor(int rpm, int steps, bool reverse) {
  int stepDelay = 60 / (nema17MotorSteps * rpm);
  int totalSteps = steps * 1;

  for (int i {0}; i < totalSteps; i++) {
    int idx = reverse ? 3 - (i % 4) : i % 4;
    Serial.println(idx);
    digitalWrite(IN1, stepSequence[idx][0]);
    digitalWrite(IN2, stepSequence[idx][1]);
    digitalWrite(IN3, stepSequence[idx][2]);
    digitalWrite(IN4, stepSequence[idx][3]);
  } 
  
  delay(stepDelay);
}

void rotateMotorCounterClockwise (bool secondMotor) {
  if (secondMotor) {
     if (millis() - lastStepTime >= stepDelay) {
      // Output reversed coil energizing pattern
      digitalWrite(IN5, reverseStepSequence[stepIndex][0]);
      digitalWrite(IN6, reverseStepSequence[stepIndex][1]);
      digitalWrite(IN7, reverseStepSequence[stepIndex][2]);
      digitalWrite(IN8, reverseStepSequence[stepIndex][3]);

      // Move to previous step (reverse)
      stepIndex = (stepIndex + 1) % 4;  // Still moves forward in array, but it's reversed sequence
      lastStepTime = millis();
      
     }
      Serial.println("moving Y motor");
      return; 
  }
  

  if (millis() - lastStepTime >= stepDelay) {
    // Output reversed coil energizing pattern
    digitalWrite(IN1, reverseStepSequence[stepIndex][0]);
    digitalWrite(IN2, reverseStepSequence[stepIndex][1]);
    digitalWrite(IN3, reverseStepSequence[stepIndex][2]);
    digitalWrite(IN4, reverseStepSequence[stepIndex][3]);

    // Move to previous step (reverse)
    stepIndex = (stepIndex + 1) % 4;  // Still moves forward in array, but it's reversed sequence
    lastStepTime = millis();
    Serial.println("moving Y motor");
  }
}

void rotateMotorClockwise (bool secondMotor) {

  if (secondMotor) {
     if (millis() - lastStepTime >= stepDelay) {
      // Output reversed coil energizing pattern
      digitalWrite(IN5, stepSequence[stepIndex][0]);
      digitalWrite(IN6, stepSequence[stepIndex][1]);
      digitalWrite(IN7, stepSequence[stepIndex][2]);
      digitalWrite(IN8, stepSequence[stepIndex][3]);

      // Move to previous step (reverse)
      stepIndex = (stepIndex + 1) % 4;  // Still moves forward in array, but it's reversed sequence
      lastStepTime = millis();
      
     }
    Serial.println("moving X motor");
    return; 
  }


  if (millis() - lastStepTime >= stepDelay) {
    // Output reversed coil energizing pattern
    digitalWrite(IN1, stepSequence[stepIndex][0]);
    digitalWrite(IN2, stepSequence[stepIndex][1]);
    digitalWrite(IN3, stepSequence[stepIndex][2]);
    digitalWrite(IN4, stepSequence[stepIndex][3]);

    // Move to previous step (reverse)
    stepIndex = (stepIndex + 1) % 4;  // Still moves forward in array, but it's reversed sequence
    lastStepTime = millis();
    Serial.println("moving X motor");
  }
}
void moveRight() {
  rotateMotorCounterClockwise(false);
}
void moveLeft() {
  rotateMotorClockwise(false);
}
void moveUp() {
  rotateMotorClockwise(true);
}
void moveDown() {
  rotateMotorCounterClockwise(true); 
}
#pragma endregion

#pragma region handle_Joystick_Control
void handleJoystick (bool printDebug, bool posDebug) {
   xVal = analogRead(JoystickX);
   yVal = analogRead(JoystickY);

    // Handle Joystick X-Axis Input        
    if (xVal > 4080) {
        rotateMotorCounterClockwise(false);
        posX++;
    } else if (xVal < 10) {
        rotateMotorClockwise(false);
        posX--;
    }

    // Handle Joystick Y-Axis Input        
    if (yVal > 4080) {
      rotateMotorClockwise(true);
      posY++;
    } else if (yVal < 10) {
      rotateMotorCounterClockwise(true);    
      posY--;
    }    

    if (posDebug) {
      Serial.print("Xpos: ");
      Serial.print(posX);
      Serial.print(", Ypos: ");
      Serial.println(posY);
    }

    if (printDebug) {
      Serial.print("xVal: ");
      Serial.print(xVal);
      Serial.print(", yVal: ");
      Serial.println(yVal);    
    }
}
#pragma endregion

void setup() {
  setPinModes();
  Serial.begin(115200);
}

void loop() {
  server.handleClient();
  handleJoystick(false, true);  
}
