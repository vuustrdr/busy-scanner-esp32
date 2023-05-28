// Version is here
int version = 1;
// Key
String authKey = "testauthkey119";
// Once i have a server to host files on i can replace it with this.
String ip = "100.97.59.135";
// Id for store
String alias = "test_10";
// Endpoints (Set up to local because why not)
String endpoint = "http://" + ip + ":3001/requests/" + alias;
String versionEndpoint = "http://" + ip + ":3001/files/venues/" + alias + "/version";
String firmwareEndpoint = "http://" + ip + ":3001/files/venues/" + alias;
String logEndpoint = "http://" + ip + ":3001/files/venues/" + alias + "/logs";