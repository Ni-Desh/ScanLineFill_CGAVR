#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>  // For usleep() on Linux/macOS
// #include <windows.h> // Uncomment for Windows (Sleep())

using namespace std;

// Define a Point struct for storing the coordinates
struct Point {
    int x, y;
};

// Custom Stack implementation for storing the intersections
struct Stack {
    int arr[100];
    int top;

    Stack() {
        top = -1;
    }

    void push(int x) {
        if (top < 99) {
            arr[++top] = x;
        }
    }

    int pop() {
        if (top >= 0) {
            return arr[top--];
        }
        return -1; // Stack is empty
    }

    bool isEmpty() {
        return top == -1;
    }

    int peek() {
        if (top >= 0) {
            return arr[top];
        }
        return -1;
    }
};

vector<Point> polygon; // Vector to store the polygon vertices
int n;  // Number of vertices
bool isPolygonComplete = false;  // Flag to indicate if the polygon is complete

// Function to draw the polygon boundary
void drawPolygon() {
    glColor3f(0, 0, 0); // Black boundary
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        glVertex2i(polygon[i].x, polygon[i].y);
    }
    glEnd();
}

// Function to fill the polygon using the Scanline Algorithm with manual stack
void scanlineFill() {
    int minY = polygon[0].y, maxY = polygon[0].y;

    // Find min and max Y values
    for (int i = 1; i < n; i++) {
        if (polygon[i].y < minY) minY = polygon[i].y;
        if (polygon[i].y > maxY) maxY = polygon[i].y;
    }

    for (int y = minY; y <= maxY; y++) {
        Stack intersections;

        // Find intersections with scanline
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            if ((polygon[i].y <= y && polygon[next].y > y) || (polygon[next].y <= y && polygon[i].y > y)) {
                int x = polygon[i].x + (y - polygon[i].y) * (polygon[next].x - polygon[i].x) / (polygon[next].y - polygon[i].y);
                intersections.push(x);

                // Print the intersection point (x, y) to CMD
                cout << "Intersection at: (" << x << ", " << y << ")\n";
            }
        }

        // Sort intersection points using a simple bubble sort
        for (int i = 0; i < intersections.top; i++) {
            for (int j = i + 1; j <= intersections.top; j++) {
                if (intersections.arr[i] > intersections.arr[j]) {
                    int temp = intersections.arr[i];
                    intersections.arr[i] = intersections.arr[j];
                    intersections.arr[j] = temp;
                }
            }
        }

        // Fill between pairs of intersections
        glColor3f(1, 0, 0); // Red fill
        glBegin(GL_LINES);
        while (!intersections.isEmpty()) {
            int x1 = intersections.pop();
            int x2 = intersections.pop();

            glVertex2i(x1, y);
            glVertex2i(x2, y);
        }
        glEnd();

        glFlush(); // Update the display

        // usleep(2000); // Slow down filling (adjust the value for speed)
        Sleep(50); // Uncomment for Windows (Sleep in milliseconds)
    }
}

// Display function (fills polygon automatically)
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPolygon();
    if (isPolygonComplete) {
        scanlineFill(); // Automatically fills the polygon once it's complete
    }
}

// OpenGL Initialization
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // White background
    glColor3f(0.0, 0.0, 0.0); // Black boundary color
    glPointSize(2.0);
    glOrtho(0, 500, 0, 500, -1, 1);
}

// Mouse click handler
void mouseClick(int button, int state, int x, int y) {
    // Check if the left mouse button is clicked and if the polygon is not complete
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !isPolygonComplete) {
        // Adjust for OpenGL's coordinate system (flipping Y-axis)
        y = 500 - y;

        // Add the clicked point to the polygon
        polygon.push_back(Point{x, y});
        n++;

        // If the polygon has enough vertices (e.g., 4 for a quadrilateral), mark it as complete
        if (n >= 3) {  // 3 vertices for a triangle, 4 for a quadrilateral, etc.
            isPolygonComplete = true;
        }

        // Redraw the window
        glutPostRedisplay();
    }
}

// Input for polygon vertices
void inputPolygon() {
    cout << "Enter the number of vertices for the polygon: ";
    cin >> n;

    cout << "Enter the coordinates of the polygon vertices (x y):\n";
    for (int i = 0; i < n; i++) {
        Point p;
        cin >> p.x >> p.y;
        polygon.push_back(p);
    }

    // After input, the user can start triggering the filling with mouse clicks
    cout << "Click inside the window to start the animation and fill the polygon!\n";
}

// Main function
int main(int argc, char** argv) {
    // Input vertices for the polygon
    inputPolygon();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Polygon Fill with Mouse Click");

    init();
    glutDisplayFunc(display);

    // Set up the mouse click event handler
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}

