// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

int tipoExibicao = 0;//0 - Front[F1] (default) | 1[f2] - Top | 2[f3] - Back 



void alterarVisao(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS){
		printf("FOI\n\n");
		switch(key){
			case GLFW_KEY_F1:
			tipoExibicao = 0;
			break;

			case GLFW_KEY_F2:
			tipoExibicao = 1;
			break;

			case GLFW_KEY_F3:
			tipoExibicao = 2;
			break;
		}
	}
    
}




int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	


	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1350, 768, "JumpPirate Game", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("obj/Mesa.obj", vertices, uvs, normals);
	bool res2 = loadOBJ("obj/Barril.obj", vertices, uvs, normals);
	bool res3 = loadOBJ("obj/Espada.obj", vertices, uvs, normals);
	bool res4 = loadOBJ("obj/Cabeca.obj", vertices, uvs, normals);
	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	

	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	int nbFrames = 0;

	int tipoExibicao = 0;
	bool isExibirPerspective = false;
	bool isExibirAnimacao = false;
	bool isAnimacaoVoltando = false;

	int deslocCamera = 0;

	do{
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime); 
		lastFrameTime = currentTime;
		nbFrames++;

		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 0.1;
			printf("\n Tipoexibicao: %d\n", tipoExibicao);
			

			if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS){
				tipoExibicao = 0;
			}

			if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS){
				tipoExibicao = 1;
			}

			if(glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS){
				tipoExibicao = 2;
			}

			if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
				if(isExibirPerspective){
					isExibirPerspective = false;
				}else{
					isExibirPerspective = true;
				}
			}

			if(glfwGetKey(window, GLFW_KEY_ENTER)){
				isExibirAnimacao = true;
			}

			if(isExibirAnimacao){
				if(deslocCamera > 50 || isAnimacaoVoltando){
					deslocCamera--;
					isAnimacaoVoltando = true;
					if(deslocCamera < 25){
						isExibirAnimacao = false;
						isAnimacaoVoltando = false;
					}
				}else{
					deslocCamera++;
				}
			}
		}
		
		//front view - DEFAULT 
		glm::mat4 ViewMatrix = glm::lookAt(
		    glm::vec3( 0, 25, 50 ), // Camera is here
		    glm::vec3( 0, 10, 0 ), // and looks here
		    glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
		);

		if(isExibirAnimacao){
			ViewMatrix = glm::lookAt(
		    glm::vec3( deslocCamera, deslocCamera, 25), // Camera is here
		    glm::vec3( 0, 10, 0 ), // and looks here
		    glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
		);
		}else{

			if(tipoExibicao == 1){
			ViewMatrix = glm::lookAt(
			glm::vec3( 0, 60, 0 ), // Camera is here
			glm::vec3( -8, 0, 0 ), // and looks here
			glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
			);
			} 
		
			if(tipoExibicao == 2){
			ViewMatrix = glm::lookAt(
			glm::vec3( 0, 25, -60 ), // Camera is here
			glm::vec3( -8, 10, 0 ), // and looks here
			glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
			);
			}
		}

		



		glm::mat4 ProjectionMatrix = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);

		// if(isExibirPerspective){
			ProjectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
		// }


		// if(isExibirAnimacao){
		// 	while(contadorAnimacao < 40){

		// 		contadorAnimacao++;
		// 	}
		// }

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input

		glm::mat4 ModelMatrix = glm::mat4(1.0);

		vec3 gPosition1(5.0f, 0.0f, 0.0f);
		glm::mat4 TranslationMatrix = translate(mat4(), gPosition1); 

		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * TranslationMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

