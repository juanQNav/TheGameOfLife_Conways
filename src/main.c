#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define VERTICAL_SIZE 1400
#define HORIZONTAL_SIZE 1400

#define NUMBER_CELLS 80
#define CELL_SIZE (1.959/NUMBER_CELLS)

//global var
int cells [NUMBER_CELLS][NUMBER_CELLS];
int cellsCopy[NUMBER_CELLS][NUMBER_CELLS];

bool pause = false;

// function prototypes
void initializeCells(int cells[NUMBER_CELLS][NUMBER_CELLS],int cellsCopy[NUMBER_CELLS][NUMBER_CELLS]);
void drawSquare(bool withFill, float x, float y );
void gameOfLife(int cells[NUMBER_CELLS][NUMBER_CELLS], int cellsCopy[NUMBER_CELLS][NUMBER_CELLS]);
void delay(int milliseconds);
void get_clicked_cell(double x_normalized, double y_normalized);
void pixel_to_normalized_coordinates(double x_pixel, double y_pixel,  double *normalized_x,  double *normalized_y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void update_cells_on_drag(double normalized_x, double normalized_y);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);



//main function
int main() {
    
    //initialize glfw
     if (!glfwInit()) {
        return -1;
    }
 
    //create a GLFWwindow object of px X px, Naming it "The Game of Life"
    GLFWwindow* window = glfwCreateWindow(VERTICAL_SIZE, HORIZONTAL_SIZE, "The Game Of Life", NULL, NULL);
    
    //error check if window fails to create
    if(window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    // Get the primary monitor
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) {
        fprintf(stderr, "Error al obtener el monitor primario\n");
        glfwTerminate();
        return -1;
    }

    // Get screen resolution
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    if (!mode) {
        fprintf(stderr, "Error al obtener el modo de video\n");
        glfwTerminate();
        return -1;
    }

    // Calculate the position of the window so that it will be centered
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    int xPos = (mode->width - windowWidth) / 2;
    int yPos = (mode->height - windowHeight) / 2;

    // Place the window in the centered position.
    glfwSetWindowPos(window, xPos, yPos);

    //introduce the window into the current context
    glfwMakeContextCurrent(window);
    
    //load GLAD so it configures OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    //specify the viwport of OpenGL in the wWindow
    //int this case the viwport goes from x = 0, y = 0, to x = 1800, y = 1000
    glViewport(0, 0, VERTICAL_SIZE, HORIZONTAL_SIZE);
    
    //specify the color of the background
    glClearColor(0.09f, 0.09f, 0.09f, 1.0f);
    
    //initilize array
    
    initializeCells(cells, cellsCopy);

    cells[5][3] = 1;
    cells[5][4] = 1;
    cells[5][5] = 1;

    cells[21][21] = 1;
    cells[22][22] = 1;
    cells[22][23] = 1;
    cells[21][23] = 1;
    cells[20][23] = 1;

    cells[31][39] = 1;
    cells[32][31] = 1;
    cells[34][31] = 1;
    cells[32][32] = 1;
    cells[33][32] = 1;
    cells[33][33] = 1;
    cells[33][34] = 1;
    cells[33][35] = 1;
    cells[35][31] = 1;
    cells[36][31] = 1;

    // Establecer la función de callback para el evento de clic
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Establecer la función de devolución de llamada para la pulsación de teclas
    glfwSetKeyCallback(window, key_callback);
    
    double lastUpdateTime = glfwGetTime();
    double updateInterval = 0.1; // Intervalo de actualización en segundos (ejemplo: 0.1 segundos)

    //main while loop
    while (!glfwWindowShouldClose(window)) 
    {
        double currentTime = glfwGetTime();
        if (!pause && (currentTime - lastUpdateTime) >= updateInterval) {
            glClearColor(0.09f, 0.09f, 0.09f, 1.0f);
            memcpy(cellsCopy, cells, sizeof(cells));
            gameOfLife(cells, cellsCopy);
            memcpy(cells, cellsCopy, sizeof(cellsCopy));

            lastUpdateTime = currentTime;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < NUMBER_CELLS; i++) {
            for (int j = 0; j < NUMBER_CELLS; j++) {
                float x = -1 * (1 - CELL_SIZE) + i * CELL_SIZE;
                float y = (1 - CELL_SIZE - 0.02) - j * CELL_SIZE;

                if (cellsCopy[i][j] == 1) {
                    drawSquare(true, x, y);
                } else {
                    drawSquare(false, x, y);
                }
            }
        }

        // Swap buffers and process events
        glfwSwapBuffers(window);
        glfwWaitEventsTimeout(updateInterval); // Espera eventos durante el intervalo de actualización
    }
 

    // delete window before ending the program
    glfwDestroyWindow(window);
    //terminate GLFW before enindg the program
    glfwTerminate();
    return 0;
}

void drawSquare(bool withFill, float x, float y ) {
    if (withFill) {
        glColor3f(0.8f, 0.8f, 0.8f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glColor3f(0.8f, 0.8f, 0.8f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBegin(GL_QUADS);
    glVertex2f(x, y + CELL_SIZE); // Vértice inferior izquierdo
    glVertex2f(x + CELL_SIZE, y + CELL_SIZE); // Vértice inferior derecho
    glVertex2f(x + CELL_SIZE, y); // Vértice superior derecho
    glVertex2f(x, y); // Vértice superior izquierdo
    glEnd();
}

void initializeCells(int cells[NUMBER_CELLS][NUMBER_CELLS],int cellsCopy[NUMBER_CELLS][NUMBER_CELLS])
{
    for(int i = 0; i < NUMBER_CELLS; i++)
        for(int j = 0; j < NUMBER_CELLS; j++)
        {
            cells[i][j] = 0;
            cellsCopy[i][j] = 0;
        }
}

void gameOfLife(int cells[NUMBER_CELLS][NUMBER_CELLS], int cellsCopy[NUMBER_CELLS][NUMBER_CELLS])
{
    int numberNeighbors = 0;
    float x = -1 * (1 - CELL_SIZE), y = (1 - CELL_SIZE - 0.02);

    for(int i = 0; i < NUMBER_CELLS; i++)
    {
        for(int j = 0; j < NUMBER_CELLS; j++)
        {   
             numberNeighbors = 0;
             numberNeighbors =   cells[(i -1) % NUMBER_CELLS][(j - 1) % NUMBER_CELLS] +\
                                 cells[(i) % NUMBER_CELLS][(j - 1) % NUMBER_CELLS] +\
                                 cells[(i + 1 ) % NUMBER_CELLS][(j - 1) % NUMBER_CELLS] +\
                                 cells[(i - 1 ) % NUMBER_CELLS][(j) % NUMBER_CELLS] +\
                                 cells[(i + 1 ) % NUMBER_CELLS][(j) % NUMBER_CELLS] +\
                                 cells[(i - 1 ) % NUMBER_CELLS][(j + 1) % NUMBER_CELLS] +\
                                 cells[(i) % NUMBER_CELLS][(j + 1) % NUMBER_CELLS] +\
                                 cells[(i + 1 ) % NUMBER_CELLS][(j + 1) % NUMBER_CELLS];
             if(cells[i][j] == 0 && numberNeighbors == 3)
                cellsCopy[i][j] = 1;
             else if(cells[i][j] == 1 && (numberNeighbors > 3 || numberNeighbors < 2))
                cellsCopy[i][j] = 0;

            if(cellsCopy[i][j] == 1)
                drawSquare(true, x, y);
            else
                drawSquare(false, x, y);

            x += CELL_SIZE;
        }
        x = -1 * (1 - CELL_SIZE);
        y -= CELL_SIZE;
    }
}

void delay(int milliseconds) 
{
    long delay_clocks = (long)(milliseconds * CLOCKS_PER_SEC / 1000);
    
    clock_t start_time = clock();
    
    while (clock() < start_time + delay_clocks) {}
}

void get_clicked_cell(double x_normalized, double y_normalized) 
{
    double cell_size = CELL_SIZE;

    double centered_x = x_normalized;
    double centered_y = y_normalized;

    int cell_x_index = (int)((centered_x + (1 - CELL_SIZE)) / cell_size);
    int cell_y_index = (int)((centered_y + (1 - CELL_SIZE)) / cell_size);

    printf("Celda tocada: (%d, %d)\n", cell_x_index, cell_y_index);

    cells[cell_x_index][cell_y_index] = 1;
    cellsCopy[cell_x_index][cell_y_index] = 1;
}

void pixel_to_normalized_coordinates(double x_pixel, double y_pixel,  double *normalized_x,  double *normalized_y) 
{
    double center_x = HORIZONTAL_SIZE / 2.0;
    double center_y = VERTICAL_SIZE / 2.0;

    double diff_x = x_pixel - center_x;
    double diff_y = y_pixel - center_y;

    *normalized_x = diff_x / center_x;
    *normalized_y = diff_y / center_y;  

}
void update_cells_on_drag(double normalized_x, double normalized_y) 
{
    double cell_size = CELL_SIZE;

    double centered_x = normalized_x;
    double centered_y = normalized_y;
    int cell_x_index = (int)((centered_x + (1 - CELL_SIZE)) / cell_size);
    int cell_y_index = (int)((centered_y + (1 - CELL_SIZE)) / cell_size);

    printf("Celda tocada: (%d, %d)\n", cell_x_index, cell_y_index);

    cells[cell_x_index][cell_y_index] = 1;
    cellsCopy[cell_x_index][cell_y_index] = 1;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
{
    double normalized_x, normalized_y;
    pixel_to_normalized_coordinates(xpos, ypos, &normalized_x, &normalized_y);
    update_cells_on_drag(normalized_x, normalized_y);

    printf("\n----->(%f, %f)\n", normalized_x, normalized_y);

    drawSquare(true, -1 * normalized_x, -1 + normalized_y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            double normalized_x, normalized_y;
            pixel_to_normalized_coordinates(xpos, ypos, &normalized_x, &normalized_y);
            update_cells_on_drag(normalized_x, normalized_y);

            printf("\n----->(%f, %f)\n", normalized_x, normalized_y);

            drawSquare(true, -1 * normalized_x, -1 + normalized_y);
            // Register the cursor position callback to track dragging
            glfwSetCursorPosCallback(window, cursor_position_callback);
        } else if (action == GLFW_RELEASE) {
            // Reset cursor position callback when mouse button is released
            glfwSetCursorPosCallback(window, NULL);
        }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pause = !pause;
        printf("Se ha presionado la tecla espacio\n");
    }
}
