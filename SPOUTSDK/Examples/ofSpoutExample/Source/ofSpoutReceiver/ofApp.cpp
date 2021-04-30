/*

	Spout OpenFrameworks Receiver example

    Visual Studio using the Spout SDK

	Copyright (C) 2021 Lynn Jarvis.

	Spout 2.007
	OpenFrameworks 10
	Visual Studio 2017

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//
	// Options
	//

	// Logging (see sender example)
	// OpenSpoutConsole(); // for debugging when a console is not availlable
	EnableSpoutLog(); // Spout logging to console

	// Specify the sender to connect to.
	// The application will not connect to any other unless the user selects one.
	// If that sender closes, the application will wait for the nominated sender to open.
	// receiver.SetReceiverName("Spout Demo Sender");

	// Disable CPU sharing backup
	// If the graphics is not compatible for OpenGL/DirectX texture sharing,
	// CPU backup methods with system memory and DirectX textures are used.
	// In most cases it is satisfactory to leave auto-detection enabled,
	// but sometimes it may be preferable to simply fail if incompatible
	// so that it is clear whether high speed texture sharing is being used.
	// receiver.SetAutoShare(false);

	ofSetWindowTitle("OpenGL Receiver Example");

	ofBackground(0, 0, 0);

	// Allocate an RGBA texture to receive from the sender
	// It is resized later to match the sender - see Update()
	myTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

	// Allocate an RGB image for this example
	// it can also be RGBA, BGRA or BGR
	myImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);

	// For sender data
	sendermousex = sendermousey = senderbutton = 0.0f;
	
} // end setup


//--------------------------------------------------------------
void ofApp::update() {
	// If IsUpdated() returns true, the sender size has changed
	// and the receiving texture or pixel buffer must be re-sized.
	if (receiver.IsUpdated()) {
		myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), GL_RGBA);
		// Also resize the image for this example
		myImage.resize(receiver.GetSenderWidth(), receiver.GetSenderHeight());
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	//
	// ReceiveTexture or ReceiveImage connect to and receive from a sender
	// Optionally include the ID of an fbo if one is currently bound
	//
	// For successful receive, sender details can be retrieved with
	//		const char * GetSenderName();
	//		unsigned int GetSenderWidth();
	//		unsigned int GetSenderHeight();
	//		DWORD GetSenderFormat();
	//		double GetSenderFps();
	//		long GetSenderFrame();
	//
	// If receive fails, the sender has closed
	// Connection can be tested at any time with 
	//		bool IsConnected();
	//

	// Option 1 : Receive texture
	if (receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget)) {

		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());

		// Example of receiving a data buffer.
		// In this case, receive mouse coordinates from the example sender.
		// Refer to the Openframeworks sender example.
		if (receiver.IsFrameNew()) {
			if (receiver.ReadMemoryBuffer(receiver.GetSenderName(), senderdata, 256)) {
				// We are expecting mouse coordinates and button status from the 
				// example graphics sender, saved as 3 byte decimal numbers
				// (3+1+3+1+3+1 = 12 bytes). For the Spout Demo sender and other examples 
				// without button status, the last 4 bytes are zero and senderbutton will be zero.
				// Here we use sscanf to convert the data to floats.
				sscanf_s(senderdata, "%f %f %f", &sendermousex, &sendermousey, &senderbutton);
			}
			else {
				sendermousex = sendermousey = senderbutton = 0.0f;
			}
		}
	}

	// Option 2 : Receive pixel data
	// Specify RGB for this example. Default is RGBA.
	/*
	if (receiver.ReceiveImage(myImage.getPixels().getData(), GL_RGB)) {
		// ofImage update is necessary because the pixels have been changed externally
		myImage.update();
		myImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	*/
	
	/*
	// Option 3 : Receive an OpenGL shared texture to access directly
	// Only if compatible for GL/DX interop, or BindSharedTexture fails
	if(receiver.ReceiveTexture()) {
		// Bind to get access to the shared texture
		if (receiver.BindSharedTexture()) {
			// Get the shared texture ID and do something with it
			GLuint texID = receiver.GetSharedTextureID();
			// For this example, copy from the shared texture 
			// if the local texture has been updated in ofApp::update()
			if ((int)myTexture.getWidth() == receiver.GetSenderWidth()
				&& (int)myTexture.getHeight() == receiver.GetSenderHeight()) {
				receiver.CopyTexture(texID, GL_TEXTURE_2D,
					myTexture.getTextureData().textureID,
					myTexture.getTextureData().textureTarget,
					receiver.GetSenderWidth(), receiver.GetSenderHeight());
			}
			// Un-bind to release access to the shared texture
			receiver.UnBindSharedTexture();
			myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		}
	}
	*/

	// Example of receiving data from the sender
	// Draw the sender mouse position if data has been received.
	// Refer to the Openframeworks sender example.
	if (receiver.IsConnected() && sendermousex > 0.0f) {

		// Draw a red ball at the sender mouse position
		ofSetColor(255, 0, 0);
		ofDrawCircle(sendermousex, sendermousey, 0, 16);

		// Draw a line if the sender mouse is dragged
		ofSetLineWidth(2.0f);
		// Accumulate points on LH button drag
		if (senderbutton == 1.0f)
			senderpoints.push_back(ofVec2f(sendermousex, sendermousey));
		// Clear points on sender RH button click
		if (senderbutton == 2.0f)
			senderpoints.clear();
		// Draw all points
		if (senderpoints.size() > 1) {
			for (int i = 0; i < (int)senderpoints.size() - 1; i++)
				ofDrawLine(senderpoints[i], senderpoints[i + 1]);
		}

	}

	// On-screen display
	showInfo();

	// To synchronise the sender to the receiver,
	// send a ready signal after rendering.
	// Refer to the sender example.
	// receiver.SetFrameSync(receiver.GetSenderName());


}


//--------------------------------------------------------------
void ofApp::showInfo() {

	std::string str;
	ofSetColor(255, 255, 255);

	if(receiver.IsConnected()) {

		// Show sender details
		str = receiver.GetSenderName(); // sender name
		str += " (";

		// Show sender sharing mode if not OpenGL compatible
		if (receiver.GetSenderCPU())
			str += "CPU share : "; 

		// Show sender size
		str += to_string(receiver.GetSenderWidth()); // width
		str += "x";
		str += to_string(receiver.GetSenderHeight()); // height 

		// Applications < 2.007 will return no frame count information
		// Frame counting can also be disabled in SpoutSettings
		if (receiver.GetSenderFrame() > 0) {
			str += " : fps ";
			str += to_string((int)(round(receiver.GetSenderFps()))); // frames per second
			str += " : frame ";
			str += to_string(receiver.GetSenderFrame()); // frame since the sender started
		}
		str += ") ";
		
		ofDrawBitmapString(str, 10, 20);
	}
	else {
		str = "No sender detected";
		ofDrawBitmapString(str, 10, 20);
	}

	// Show more details if not OpenGL/DirectX compatible
	if (!receiver.IsGLDXready()) {
		if (receiver.GetAutoShare()) {
			// CPU share allowed (default)
			str = "CPU share receiver";
		}
		else {
			// CPU share disabled (program setting)
			str = "Graphics not texture share compatible";
		}
		ofDrawBitmapString(str, 10, 35);

		// Show current graphics adapter
		str = "Graphics adapter ";
		str += to_string(receiver.GetAdapter());
		str += " : ";
		str += receiver.AdapterName();
		ofDrawBitmapString(str, 10, 50);
	}

}

//--------------------------------------------------------------
void ofApp::exit() {
	receiver.ReleaseReceiver();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if(button == 2) { // rh button
		// Open the sender selection panel
		// Spout must have been installed
		receiver.SelectSender();
	}
}

