from flask import Flask,request, render_template,jsonify
import json
app = Flask(__name__)
beta={'speed':10}
request_data=None

@app.route("/",methods=['GET'])
def hello_world():
    if request.method == 'GET':
        return render_template("index.html")

@app.route("/about",methods=['GET','POST'])
def world():
    if request.method == 'GET':
        return beta
    

@app.route('/jsonexample', methods=['GET','POST'])
def json_example():
    global beta 
    if request.method == 'GET':
        message = {'greeting':'Hello from Flask!'}
        return jsonify(beta)  # serialize and use JSON headers
    if request.method == 'POST':
        global request_data
        request_data = request.get_json()
        beta= request_data
        return request_data

