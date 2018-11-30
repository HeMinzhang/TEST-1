#!/usr/bin/env python3

import os, json, copy, jsonschema, binascii, argparse, signal
import jsonschema
from bottle import Bottle, route, run, static_file, error, abort, request, template, HTTPError
import traceback

app=Bottle()

class globals:
    host = None
    port = None
    config_file = None
    pid_file = None
    debug = False
    token = "gwconfig"
    static =  "static"
    config_schema = None

def main():
    # Command line arguments
    parser = argparse.ArgumentParser(
        description = "SELink gateway configuration web UI")
    parser.add_argument("--host",
        default = "[::1]",
        help = "Host on which the web UI server will listen")
    parser.add_argument("--port",
        default = 10201,
        help =  "Port on which the web UI server will listen")
    parser.add_argument("-c", "--config",
        default = "/opt/selink/selinkgw.json",
        help = "Path to gateway configuration file")
    parser.add_argument("--pidfile",
        default = "/var/run/selinkgw.pid",
        help = "Path to gateway pidfile. Set to empty string to disable daemon reload")
    parser.add_argument("--use-token",
        action = "store_true",
        help = "Generate a random token to restrict access to the web UI")
    parser.add_argument("--debug",
        action = "store_true",
        help = "Enable debug mode")
    args = parser.parse_args()
    
    globals.host = args.host
    globals.port = args.port
    globals.config_file = args.config
    globals.pid_file = args.pidfile
    globals.debug = args.debug
    if args.use_token:
        globals.token = binascii.hexlify(os.urandom(16)).decode('utf-8')
    
    # load schema for config validation
    with open("config-schema.json", 'r') as f:
        globals.config_schema = json.load(f)
    

# EditableGrid json template
TABLE={
    "metadata":[
        {"name":"description","label":"Name","datatype":"string","editable":True},
        {"name":"listen-port","label":"Listen Port","datatype":"integer(,,,,)","editable":True},
        {"name":"redirect-host","label":"Redirect Host","datatype":"string","editable":True},
        {"name":"redirect-port","label":"Redirect Port","datatype":"integer(,,,,)","editable":True},
        {"name":"key-id","label":"Key ID","datatype":"integer(,,,,)","editable":True},
        {"name":"action","label":"Delete","datatype":"html","editable":False}
    ],
    "data": []
}

# Exception to propagate an error state caused by an API operation
class APIError(Exception):
    pass

# Middleware to transform APIError exception into json error response 
def ExceptionMiddleware(func):
    def wrapper(*args,**kwargs):
        try:
            return func(*args,**kwargs)
        except APIError as e:
            # Transform API error into json error response
            if globals.debug:
                traceback.print_exc()
            return {"err": 1, "msg": str(e) }
        except HTTPError as e:
            # pass bottle exceptions down to bottle
            raise e
        except Exception as e:
            # any other exception will produce http state 400
            if globals.debug:
                traceback.print_exc()
            abort(400, "Bad Request")
    return wrapper

def check_token(token):
    if token != globals.token:
        raise APIError("Authentication Failed")

# Serve static resources
@app.route('/%s/<path:path>' % globals.static)
def res(path):
    return static_file(path, root = globals.static)

# AJAX set config
@app.route('/set/<token>', method = 'POST')
def config_set(token):
    check_token(token)
    try:
        data = json.loads(request.body.read().decode("utf-8"))
    except:
        raise APIError("New config is invalid, corrupted data")
    try:
        jsonschema.validate(data, globals.config_schema)
    except jsonschema.ValidationError as e:
        raise APIError("New config is invalid, %s"%e.message)
    try:
        with open(globals.config_file, 'w') as config_file:
            json.dump(data, config_file, indent = 4)
    except:
        raise APIError("Cannot write to config file")
    
    if globals.pid_file != "":
        try:
            pid=int(open(globals.pid_file,'r').readlines()[0]);
            os.kill(pid, signal.SIGHUP)
        except:
            print("Cannot signal process")
    
    return {"err": 0}

# AJAX get config
@app.route('/get/<token>')
def config_get(token):
    check_token(token)
    try:
        with open(globals.config_file, 'r') as config_file:
            data = json.load(config_file)
    except:
        raise APIError("Cannot read from config file")
    try:
        jsonschema.validate(data, globals.config_schema)
    except jsonschema.ValidationError as e:
        raise APIError("Existing config is invalid: %s"%e.message)
    table=copy.deepcopy(TABLE)
    rules=data["map"]
    table["data"]=[{"id": i+1, "values": rules[i]} for i in range(len(rules))]
    return table

# Main page
@app.route('/<token>')
def home(token):
    if token != globals.token:
        abort(404)
    return template('main', token = globals.token, static = globals.static)

# Application entry point
if __name__ == "__main__":
    main()
    print("http://%s:%d/%s"%(globals.host, globals.port, globals.token))
    app.install(ExceptionMiddleware)
    app.run(
        host = globals.host.lstrip("[").rstrip("]"),
        port = globals.port,
        debug = globals.debug,
        quiet = not globals.debug,
        threaded = True)

