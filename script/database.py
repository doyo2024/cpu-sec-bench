
# python library for configuration and result database operations

import json

# databases
cfgDB = {}
resultDB = {}

def open_db(fname):
    with open(fname, "r") as read_file:
        return json.loads(read_file.read())

def write_db(fname, db):
    with open(fname, "w") as write_file:
        json.dump(db, write_file, indent=2, separators=(',', ': '), sort_keys=True)

def db_get_param(db, test, param, default_value):
    if not test in db:
        return default_value
    if not param in db[test]:
        return default_value
    return db[test][param]

def db_set_param(db, test, param, value):
    if not test in db:
        db[test] = {}
    db[test][param] = value

def open_cfg_db():
    global cfgDB
    cfgDB = open_db("../configure.json")

def write_result_db():
    global resultDB
    write_db("results.json", resultDB)

def write_result():
    global resultDB
    cases = sorted(resultDB)
    result = open("results.dat", 'w')
    for c in cases:
        result.write(c + " " + str(resultDB[c]["result"]) + "\n");
    result.close()

def cfg_get_arguments(test):
    global cfgDB
    return db_get_param(cfgDB, test, "arguments", "")

def cfg_get_prog(test):
    global cfgDB
    return db_get_param(cfgDB, test, "program", test)

def cfg_get_depends(test):
    global cfgDB
    return db_get_param(cfgDB, test, "require", [])

def cfg_get_expected_results(test):
    global cfgDB
    return db_get_param(cfgDB, test, "results", {})

def cfg_get_tests():
    global cfgDB
    return cfgDB.keys()

def result_get_result(test):
    global resultDB
    global cfgDB
    if not test in cfgDB:
        return -2
    return db_get_param(resultDB, test, "result", -1)

def result_record_result(test, v):
    global resultDB
    db_set_param(resultDB, test, "result", v)

