#include "ZooidManager.h"

//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//
//------------------------ ZOOID METHODS -----------------------//
//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////


//--------------------------------------------------------------
Zooid::Zooid(){
    id = 0;
    radius = 0.01f;
    currentOrientation = 0.0f;
    targetOrientation = 0.0f;
    currentPosition = ofVec2f(0.0f);
    destination = ofVec2f(0.0f);
    state = 0;
    goalReached = true;
    color = ofColor::black;
    activated = true;
    reassignable = true;
    speed = 100;

    newOrientation = false;
    newColor = false;
    newDestination = false;
    newSpeed = false;
    newReassignable = false;
    newActivated = false;
}

//--------------------------------------------------------------
Zooid::~Zooid(){
    
}

//--------------------------------------------------------------
Zooid::Zooid(unsigned int _id, float _radius, float _orientation, ofVec2f _currentPosition, ofVec2f _destination,
             int _state, bool _goalReached, ofColor _color, bool _activated, bool _reassignable, unsigned int _speed)
{
    id = _id;
    radius = _radius;
    currentOrientation = _orientation;
    currentPosition = _currentPosition;
    destination = _destination;
    state = _state;
    goalReached = _goalReached;
    color = _color;
    activated = _activated;
    reassignable = _reassignable;
    speed = _speed;
    
    newOrientation = false;
    newColor = false;
    newDestination = false;
    newSpeed = false;
    newReassignable = false;
    newActivated = false;
}

//--------------------------------------------------------------
bool Zooid::operator==(const Zooid& r){
    return  this->id == r.id;
}

//--------------------------------------------------------------
bool Zooid::operator!=(const Zooid& r) {
    return !(*this == r);
}

//--------------------------------------------------------------
Zooid& Zooid::operator=(const Zooid& other) // copy assignment
{
    if (this != &other) { // self-assignment check expected
        id = other.id;
        radius = other.radius;
        currentOrientation = other.currentOrientation;
        currentPosition = other.currentPosition;
        destination = other.destination;
        state = other.state;
        goalReached = other.goalReached;
        color = other.color;
        activated = other.activated;
        reassignable = other.reassignable;
        speed = other.speed;
    }
    return *this;
}

//--------------------------------------------------------------
ofVec2f Zooid::getPosition() {
    return currentPosition;
}

//--------------------------------------------------------------
ofVec2f Zooid::getDestination(){
    return destination;
}

//--------------------------------------------------------------
float Zooid::getOrientation() {
    return currentOrientation;
}

//--------------------------------------------------------------
int Zooid::getState() {
    return state;
}

//--------------------------------------------------------------
ofColor Zooid::getColor() {
    return color;
}

//--------------------------------------------------------------
bool Zooid::isAtDestination() {
    return goalReached;
}

//--------------------------------------------------------------
bool Zooid::isActivated() {
    return activated;
}

//--------------------------------------------------------------
unsigned int Zooid::getId() {
    return id;
}

//--------------------------------------------------------------
float Zooid::getRadius() {
    return radius;
}

//--------------------------------------------------------------
void Zooid::setDestination(float _x, float _y) {
    destination.x = _x;
    destination.y = _y;
    newDestination = true;
}

//--------------------------------------------------------------
void Zooid::setDestination(ofVec2f _destination) {
    destination = _destination;
    newDestination = true;
}

//--------------------------------------------------------------
void Zooid::setColor(ofColor _color) {
    color = _color;
    newColor = true;
}

//--------------------------------------------------------------
void Zooid::setColor(uint8_t _r, uint8_t _g, uint8_t _b) {
    color.r = _r;
    color.g = _g;
    color.b = _b;
    newColor = true;
}

//--------------------------------------------------------------
bool Zooid::isReassignable() {
    return reassignable;
}

//--------------------------------------------------------------
void Zooid::setReassignable(bool _reassignable) {
    reassignable = _reassignable;
    newReassignable = true;
}

//--------------------------------------------------------------
void Zooid::setOrientation(float _orientation) {
    targetOrientation = _orientation;
    newOrientation = true;
}

//--------------------------------------------------------------
void Zooid::activate() {
    activated = true;
    newActivated = true;
}

//--------------------------------------------------------------
void Zooid::deactivate() {
    activated = false;
    newActivated = true;
}

//--------------------------------------------------------------
void Zooid::setSpeed(unsigned int _speed) {
    speed = _speed;
    newSpeed = true;
}

//--------------------------------------------------------------
unsigned int Zooid::getSpeed() {
    return speed;
}

//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//
//-------------------- ZOOID MANAGER METHODS -------------------//
//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
ZooidManager::ZooidManager(){
    initialized = false;
    realCoordinates = false;
    dimensionX = dimensionY = 1.0f;;
    
    udpSender.Create();
    udpReceiver.Create();
    assignmentMode = NaiveAssignment;
    
    myZooids = vector<Zooid>();
}

//--------------------------------------------------------------
ZooidManager::~ZooidManager(){

}

//--------------------------------------------------------------
void ZooidManager::initialize(float width, float height){

    windowWidth = width;
    windowHeight = height;
    
	udpSender.Connect("127.0.0.1", 11998);
	udpSender.SetNonBlocking(true);

	udpReceiver.Bind(11999);
	udpReceiver.SetNonBlocking(true);
}

//--------------------------------------------------------------
bool ZooidManager::sendUpdates() {
    if(initialized){
    	StringBuffer s;
    	Writer<StringBuffer> writer(s);
	
    	writer.StartObject();
    	{
    	    writer.Key("ass");          //assignation strategy
    	    writer.Int(assignmentMode);
    	    
    	    writer.Key("nb");           //number of zooids
    	    writer.Int((unsigned int)myZooids.size());
	
    	    writer.Key("zoo");
    	    writer.StartArray();        //array of zooids
    	    {
    	        for (unsigned int i = 0; i < myZooids.size(); i++) {
    	            writer.StartObject();
                    {
    	            	writer.Key("id");   //zooid id
    	            	writer.Int(myZooids[i].getId());
    	            	if(myZooids[i].newOrientation){
    	            		writer.Key("ang");  //zooid angle
    	            		writer.Double(myZooids[i].targetOrientation);
                            myZooids[i].newOrientation = false;
    	        		}
    	        		if(myZooids[i].newDestination){
		            	    writer.Key("des");  //zooid destination
		            	    writer.StartArray();
		            	    writer.Double(myZooids[i].getDestination().x);
		            	    writer.Double(myZooids[i].getDestination().y);
		            	    writer.EndArray();
                            myZooids[i].newDestination = false;
    	        		}
    	        		if(myZooids[i].newColor){
		            	    writer.Key("col"); //zooid color
		            	    writer.StartArray();
		            	    writer.Int(myZooids[i].getColor().r);
		            	    writer.Int(myZooids[i].getColor().g);
		            	    writer.Int(myZooids[i].getColor().b);
		            	    writer.EndArray();
                            myZooids[i].newColor = false;
    	        		}
    	            	if(myZooids[i].newActivated){
		            	    writer.Key("act");   //zooid activated
		            	    writer.Bool(myZooids[i].isActivated());
                            myZooids[i].newActivated = false;
    	       			}
    	            	if(myZooids[i].newReassignable){
		            	    writer.Key("rea");  //zooid reassignable
		            	    writer.Bool(myZooids[i].isReassignable());
                            myZooids[i].newReassignable = false;
    	        		}
    	        		if(myZooids[i].newSpeed){
		            	    writer.Key("vel");  //zooid speed
		            	    writer.Int(myZooids[i].getSpeed());
                            myZooids[i].newSpeed = false;
    	        		}
                    }
    	            writer.EndObject();
    	        }
    	    }
    	    writer.EndArray();
    	}
    	writer.EndObject();
	
    	int bytesSent = udpSender.Send(s.GetString(), (unsigned int)s.GetSize());

        
        
    	return bytesSent != SOCKET_ERROR ? true : false;
    }
    else
        return false;
}

//--------------------------------------------------------------
bool ZooidManager::receiveInformation(){
    char udpMessage[100000] = { 0 };
    Document zooidData;

    if (udpReceiver.PeekReceive() > 0) {
		while (udpReceiver.PeekReceive() > 0) {
			int nbBytesReceived = udpReceiver.Receive(udpMessage, 100000);

            if(zooidData.ParseInsitu(udpMessage).HasParseError())
                return false;

            int nbZooids = zooidData["nb"].GetInt();
            assignmentMode = (AssignmentMode)zooidData["ass"].GetInt();

            if (!initialized) {     //dimensions of the projection space for position mapping
                dimensionX = (float) zooidData["dim"][0].GetDouble();
                dimensionY = (float) zooidData["dim"][1].GetDouble();
                initialized = true;
            }
            
            const Value& receivedZooids = zooidData["zoo"]; // Using a reference for consecutive access is handy and faster.
            assert(receivedZooids.IsArray());
            
            if(receivedZooids.Size() == nbZooids) {
                for (SizeType i = 0; i < receivedZooids.Size(); i++) {
                    Zooid tmpZooid(receivedZooids[i]["id"].GetInt(),
                           (float) receivedZooids[i]["siz"].GetDouble(),
                           (float) receivedZooids[i]["ang"].GetDouble(),
                   ofVec2f((float) receivedZooids[i]["pos"][0].GetDouble(),
                           (float) receivedZooids[i]["pos"][1].GetDouble()),
                   ofVec2f((float) receivedZooids[i]["des"][0].GetDouble(),
                           (float) receivedZooids[i]["des"][1].GetDouble()),
                                   receivedZooids[i]["sta"].GetInt(),
                                   receivedZooids[i]["her"].GetBool(),
                           ofColor(receivedZooids[i]["col"][0].GetInt(),
                                   receivedZooids[i]["col"][1].GetInt(),
                                   receivedZooids[i]["col"][2].GetInt()),
                                   receivedZooids[i]["act"].GetBool(),
                                   receivedZooids[i]["rea"].GetBool(),
                                   receivedZooids[i]["vel"].GetInt());
                    bool zooidFound = false;

                    unsigned int tmpId = tmpZooid.getId();
                    auto it = find_if(myZooids.begin(), myZooids.end(),
                                      [&tmpId](Zooid &z) { return z.getId() == tmpId; });
                    if (it != myZooids.end())
                        *it = tmpZooid;
                    else
                        myZooids.push_back(tmpZooid);
                }
            }
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ZooidManager::updateZooid(unsigned int id, ofVec2f destination, float orientation, ofColor color, bool reassignable) {
    if (id < myZooids.size())
    {
        if(realCoordinates)
            myZooids[id].setDestination(destination);
        else
            myZooids[id].setDestination(ofMap(destination.x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(destination.y, 0.0f, windowHeight, dimensionY, 0.0f));
        
        myZooids[id].setOrientation(orientation);
        myZooids[id].setColor(color);
        myZooids[id].setReassignable(reassignable);
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::updateZooid(unsigned int id, ofVec2f destination, float orientation) {
    if (id < myZooids.size())
    {
        if(realCoordinates)
            myZooids[id].setDestination(destination);
        else
            myZooids[id].setDestination(ofMap(destination.x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(destination.y, 0.0f, windowHeight, dimensionY, 0.0f));

        myZooids[id].setOrientation(orientation);
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::updateZooid(unsigned int id, ofVec2f destination, ofColor color) {
    if (id < myZooids.size())
    {
        if(realCoordinates)
            myZooids[id].setDestination(destination);
        else
            myZooids[id].setDestination(ofMap(destination.x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(destination.y, 0.0f, windowHeight, dimensionY, 0.0f));

        myZooids[id].setColor(color);
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::updateZooid(unsigned int id, ofVec2f destination, ofColor color, float orientation, unsigned int speed, bool reassignable) {
    if (id < myZooids.size())
    {
        if(realCoordinates)
            myZooids[id].setDestination(destination);
        else
            myZooids[id].setDestination(ofMap(destination.x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(destination.y, 0.0f, windowHeight, dimensionY, 0.0f));

        myZooids[id].setColor(color);
        myZooids[id].setOrientation(orientation);
        myZooids[id].setSpeed(speed);
        myZooids[id].setReassignable(reassignable);
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::updateZooid(unsigned int id, ofColor color, bool activated) {
    if (id < myZooids.size())
    {
        myZooids[id].setColor(color);
        if(activated)
            myZooids[id].activate();
        else
            myZooids[id].deactivate();

        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::moveZooid(unsigned int id, ofVec2f position) {
    if (id < myZooids.size()) {
        if(realCoordinates)
            myZooids[id].setDestination(position);
        else
            myZooids[id].setDestination(ofMap(position.x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(position.y, 0.0f, windowHeight, dimensionY, 0.0f));

        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::moveZooid(unsigned int id, float x, float y) {
    if (id < myZooids.size()) {
        if(realCoordinates)
            myZooids[id].setDestination(x, y);
        else
            myZooids[id].setDestination(ofMap(x, 0.0f, windowWidth, dimensionX, 0.0f), ofMap(x, 0.0f, windowHeight, dimensionY, 0.0f));
        return true;
    }
    return false;
}

//--------------------------------------------------------------
ofVec2f ZooidManager::getZooidPosition(unsigned int id) {
    if (id < myZooids.size()){
        if(realCoordinates)
            return myZooids[id].getPosition();
        else
            return ofVec2f(ofMap(myZooids[id].getPosition().x, 0.0f, dimensionX, windowWidth, 0.0f), ofMap(myZooids[id].getPosition().y, 0.0f, dimensionY, windowHeight, 0.0f));
    }
    else
        return ofVec2f(0.0f);
}

//--------------------------------------------------------------
ofVec2f ZooidManager::getZooidDestination(unsigned int id) {
    if (id < myZooids.size()){
        if(realCoordinates)
            return myZooids[id].getDestination();
        else
            return ofVec2f(ofMap(myZooids[id].getDestination().x, 0.0f, dimensionX, windowWidth, 0.0f), ofMap(myZooids[id].getPosition().y, 0.0f, dimensionY, windowHeight, 0.0f));
    }
    else
        return ofVec2f(0.0f);
}

//--------------------------------------------------------------
void ZooidManager::setZooidColor(unsigned int id, ofColor c) {
     if (id < myZooids.size())
         myZooids[id].setColor(c);
}

//--------------------------------------------------------------
ofColor ZooidManager::getZooidColor(unsigned int id) {
     if (id < myZooids.size())
         return myZooids[id].getColor();
    else
        return ofColor::black;
}

//--------------------------------------------------------------
float ZooidManager::getZooidOrientation(unsigned int id) {
    if (id < myZooids.size())
        return myZooids[id].getOrientation();
    else
        return 0.0f;
}

//--------------------------------------------------------------
void ZooidManager::setZooidOrientation(unsigned int id, float _orientation) {
    if (id < myZooids.size()) {
        myZooids[id].setOrientation(_orientation);
    }
}

//--------------------------------------------------------------
int ZooidManager::getNbZooids() {
    return myZooids.size();
}

//--------------------------------------------------------------
bool ZooidManager::isInitialized() {
    return initialized;
}

//--------------------------------------------------------------
void ZooidManager::setAssignmentMode(AssignmentMode mode) {
    assignmentMode = mode;
}
//--------------------------------------------------------------
AssignmentMode ZooidManager::getAssignmentMode() {
    return assignmentMode;
}

//--------------------------------------------------------------
void ZooidManager::setZooidSpeed(unsigned int id, unsigned int speed) {
    if (id < myZooids.size()) {
        myZooids[id].setSpeed(speed);
    }
}

//--------------------------------------------------------------
unsigned int ZooidManager::getZooidSpeed(unsigned int id) {
    if (id < myZooids.size()) {
        return myZooids[id].getSpeed();
    }
    return 0;
}

//--------------------------------------------------------------
float ZooidManager::getZooidSize(unsigned int id) {
    if (id < myZooids.size()) {
        if(realCoordinates)
            return myZooids[id].getRadius();
        else
            return ofMap(myZooids[id].getRadius(), 0.0f, dimensionX, 0.0f, windowWidth);
    }
    else
        return 0.0f;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidTouched(unsigned int id) {
    return id < myZooids.size() && (myZooids[id].getState() & (1 << 0)) > 0;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidBlinded(unsigned int id) {
    return id < myZooids.size() && (myZooids[id].getState() & (1 << 1)) > 0;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidTapped(unsigned int id) {
    return id < myZooids.size() && (myZooids[id].getState() & (1 << 2)) > 0;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidShaken(unsigned int id) {
    return id < myZooids.size() && (myZooids[id].getState() & (1 << 3)) > 0;
}

//--------------------------------------------------------------
void ZooidManager::setZooidReassignable(unsigned int id, bool _reassignable) {
    if (id < myZooids.size())
        myZooids[id].setReassignable(_reassignable);
}

//--------------------------------------------------------------
void ZooidManager::activateZooid(unsigned int id) {
    if (id < myZooids.size())
        myZooids[id].activate();
}

//--------------------------------------------------------------
void ZooidManager::deactivateZooid(unsigned int id) {
    if (id < myZooids.size())
       myZooids[id].deactivate();
}

//--------------------------------------------------------------
void ZooidManager::useRealWorldCoordinates() {
    realCoordinates = true;
}

//--------------------------------------------------------------
void ZooidManager::useWindowCoordinates() {
    realCoordinates = false;
}

//--------------------------------------------------------------
void ZooidManager::setWindowSize(float w, float h) {
    windowWidth = w;
    windowHeight = h;
}

//--------------------------------------------------------------
float ZooidManager::getRealWorldWidth() {
    return dimensionX;
}

//--------------------------------------------------------------
float ZooidManager::getRealWorldHeight(){
    return dimensionY;
}

//--------------------------------------------------------------
vector<Zooid>* ZooidManager::getZooids() {
    return &myZooids;
}
