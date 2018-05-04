#include "../../nclgl/window.h"
#include "Renderer.h"
#include <ctime>

#pragma comment(lib, "nclgl.lib")

double clockToMilliseconds(clock_t ticks) {
	// units/(units/time) => time (seconds) * 1000 = milliseconds
	return (ticks / (double)CLOCKS_PER_SEC)*1000.0;
}

int main() {	
	Window w("Weather Systems", 800 ,600, false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	double lastTime = (w.GetTimer()->GetMS()) * 0.001;
	int nbFrames = 0;
	int seconds = 0;
	double frameTime = 0;
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		// http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/

		// Measure speed
		double currentTime = (w.GetTimer()->GetMS()) * 0.001;
		nbFrames++;

		// Frame time and frames per second
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
											 // printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			frameTime = frameTime + (1000.0 / double(nbFrames));
			cout << "FPS: " << (nbFrames) << std::endl;
			nbFrames = 0;
			lastTime += 1.0;
			seconds++;
			if (seconds == 300)
			{
				cout << "Total frame time (ms): " << frameTime << std::endl;
				cout << "Average frame time over 5 mins: " << (frameTime / 300 ) << std::endl;
				cout << "5 min end" << std::endl;
				//std::cout << "seconds: " << seconds / 60 << std::endl;
			}
		}

		float msec = w.GetTimer()->GetTimedMS();
		renderer.UpdateScene(msec);
		renderer.RenderScene();
	}
	return 0;
}