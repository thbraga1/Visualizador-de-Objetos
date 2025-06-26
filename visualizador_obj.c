#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int win_id;

// Estruturas para armazenar dados do modelo .obj
typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    float x, y, z;
} Normal;

typedef struct {
    int v1, v2, v3;      
    int n1, n2, n3;     
    int hasNormals;      
} Face;

// Arrays dinâmicos para armazenar dados do modelo
Vertex *vertices = NULL;
Normal *normals = NULL;
Face *faces = NULL;
int numVertices = 0;
int numNormals = 0;
int numFaces = 0;

// Variáveis para escala automática
float minX, maxX, minY, maxY, minZ, maxZ;
float scaleFactor = 1.0f;

// Função para liberar memória
void freeModel() {
    if (vertices) {
        free(vertices);
        vertices = NULL;
    }
    if (normals) {
        free(normals);
        normals = NULL;
    }
    if (faces) {
        free(faces);
        faces = NULL;
    }
    numVertices = numNormals = numFaces = 0;
}

// Função para calcular a escala automática
void calculateScale() {
    if (numVertices == 0) return;
    
    minX = maxX = vertices[0].x;
    minY = maxY = vertices[0].y;
    minZ = maxZ = vertices[0].z;
    
    for (int i = 1; i < numVertices; i++) {
        if (vertices[i].x < minX) minX = vertices[i].x;
        if (vertices[i].x > maxX) maxX = vertices[i].x;
        if (vertices[i].y < minY) minY = vertices[i].y;
        if (vertices[i].y > maxY) maxY = vertices[i].y;
        if (vertices[i].z < minZ) minZ = vertices[i].z;
        if (vertices[i].z > maxZ) maxZ = vertices[i].z;
    }
    
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;
    float maxRange = fmax(fmax(rangeX, rangeY), rangeZ);
    
    // Escala para que o modelo caiba em uma caixa de tamanho 60 (considerando viewport de -50 a 50)
    if (maxRange > 0) {
        scaleFactor = 60.0f / maxRange;
    }
}

// Função para carregar arquivo .obj
int loadOBJ(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro: não foi possível abrir o arquivo %s\n", filename);
        return 0;
    }
    
    freeModel(); 
    
    char line[256];
    int vertexCapacity = 100, normalCapacity = 100, faceCapacity = 100;
    
    // Aloca memória inicial
    vertices = malloc(vertexCapacity * sizeof(Vertex));
    normals = malloc(normalCapacity * sizeof(Normal));
    faces = malloc(faceCapacity * sizeof(Face));
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            if (numVertices >= vertexCapacity) {
                vertexCapacity *= 2;
                vertices = realloc(vertices, vertexCapacity * sizeof(Vertex));
            }
            sscanf(line, "v %f %f %f", &vertices[numVertices].x, 
                   &vertices[numVertices].y, &vertices[numVertices].z);
            numVertices++;
        }
        else if (line[0] == 'v' && line[1] == 'n') {
            if (numNormals >= normalCapacity) {
                normalCapacity *= 2;
                normals = realloc(normals, normalCapacity * sizeof(Normal));
            }
            sscanf(line, "vn %f %f %f", &normals[numNormals].x, 
                   &normals[numNormals].y, &normals[numNormals].z);
            numNormals++;
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            if (numFaces >= faceCapacity) {
                faceCapacity *= 2;
                faces = realloc(faces, faceCapacity * sizeof(Face));
            }
            
            Face *face = &faces[numFaces];
            face->hasNormals = 0;
            
            // Tenta ler face com normais (formato: v//vn)
            if (sscanf(line, "f %d//%d %d//%d %d//%d", 
                      &face->v1, &face->n1, &face->v2, &face->n2, &face->v3, &face->n3) == 6) {
                face->hasNormals = 1;
            }
            // Tenta ler face com textura e normais (formato: v/vt/vn)
            else if (sscanf(line, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d", 
                           &face->v1, &face->n1, &face->v2, &face->n2, &face->v3, &face->n3) == 6) {
                face->hasNormals = 1;
            }
            // Lê apenas vértices (formato: v)
            else if (sscanf(line, "f %d %d %d", &face->v1, &face->v2, &face->v3) == 3) {
                face->hasNormals = 0;
            }
            // Tenta ler face com textura (formato: v/vt)
            else if (sscanf(line, "f %d/%*d %d/%*d %d/%*d", &face->v1, &face->v2, &face->v3) == 3) {
                face->hasNormals = 0;
            }
            else {
                continue; 
            }
            
            // Converte índices de 1-based para 0-based
            face->v1--; face->v2--; face->v3--;
            if (face->hasNormals) {
                face->n1--; face->n2--; face->n3--;
            }
            
            numFaces++;
        }
    }
    
    fclose(file);
    
    printf("Modelo carregado: %d vértices, %d normais, %d faces\n", 
           numVertices, numNormals, numFaces);
    
    calculateScale();
    printf("Fator de escala: %.3f\n", scaleFactor);
    
    return 1;
}

// Função para renderizar o modelo
void renderModel() {
    if (numFaces == 0) return;
    
    glPushMatrix();
    glScalef(scaleFactor, scaleFactor, scaleFactor);
    
    // Centraliza o modelo
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;
    glTranslatef(-centerX, -centerY, -centerZ);
    
    glBegin(GL_TRIANGLES);
    
    for (int i = 0; i < numFaces; i++) {
        Face *face = &faces[i];
        
        // Vértice 1
        if (face->hasNormals && face->n1 >= 0 && face->n1 < numNormals) {
            glNormal3f(normals[face->n1].x, normals[face->n1].y, normals[face->n1].z);
        }
        if (face->v1 >= 0 && face->v1 < numVertices) {
            glVertex3f(vertices[face->v1].x, vertices[face->v1].y, vertices[face->v1].z);
        }
        
        // Vértice 2
        if (face->hasNormals && face->n2 >= 0 && face->n2 < numNormals) {
            glNormal3f(normals[face->n2].x, normals[face->n2].y, normals[face->n2].z);
        }
        if (face->v2 >= 0 && face->v2 < numVertices) {
            glVertex3f(vertices[face->v2].x, vertices[face->v2].y, vertices[face->v2].z);
        }
        
        // Vértice 3
        if (face->hasNormals && face->n3 >= 0 && face->n3 < numNormals) {
            glNormal3f(normals[face->n3].x, normals[face->n3].y, normals[face->n3].z);
        }
        if (face->v3 >= 0 && face->v3 < numVertices) {
            glVertex3f(vertices[face->v3].x, vertices[face->v3].y, vertices[face->v3].z);
        }
    }
    
    glEnd();
    glPopMatrix();
}

//Aqui se capturam as teclas comuns (letras, números e ESC, por exemplo)
void myKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'R': 
        case 'r':
            glColor3f(1.0f, 0.0f, 0.0f);
            break;
        case 'G':
        case 'g':
            glColor3f(0.0f, 1.0f, 0.0f);
            break;
        case 'B':
        case 'b':
            glColor3f(0.0f, 0.0f, 1.0f);
            break;
        case 'L':
        case 'l':
            {
                printf("Digite o nome do arquivo .obj (ex: alligator.obj): ");
                char filename[256];
                if (fgets(filename, sizeof(filename), stdin)) {
                    filename[strcspn(filename, "\n")] = 0;
                    if (loadOBJ(filename)) {
                        printf("Modelo carregado com sucesso!\n");
                    }
                }
            }
            break;
        case 27: 
            freeModel();
            glutDestroyWindow(win_id);
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void myKeyboardSpecial(int key, int x, int y ) {
    switch ( key ) {
        case GLUT_KEY_UP:               
            glutFullScreen ( );              
            break;
        case GLUT_KEY_DOWN:                 
            glutReshapeWindow ( 640, 480 );   
            break;
        default:
            printf("Você apertou a tecla especial código: %d\n", key);    
            break;
    }
}

// Função callback chamada para gerenciar eventos do mouse
void myMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {
            float r, g, b;
            r = (double)rand() / (double)RAND_MAX;
            g = (double)rand() / (double)RAND_MAX;
            b = (double)rand() / (double)RAND_MAX;
            glColor3f(r,g,b);
            printf("%.2f, %.2f, %.2f, na posicao %d, %d\n", r, g, b, x, y);
        }
    glutPostRedisplay();
}

void init(void) 
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };

    // Passa os parâmetros definidos acima para a OpenGL
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Habilita a luz ambiente, a luz zero e o depth buffer
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    //Primeira cor, para depois mudarmos com os eventos
    glColor3f(0.5, 1.0, 0.5);
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 30.0f, 50.0f, 0.0f, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
    // Renderiza o modelo .obj se carregado, senão renderiza o bule padrão
    if (numFaces > 0) {
        renderModel();
    } else {
        glutSolidTeapot(10.0);
    }
    
    glFlush();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho (-50, 50, -50*(GLfloat)h/(GLfloat)w, 50*(GLfloat)h/(GLfloat)w, -100.0, 100.0);
    else
        glOrtho (-50*(GLfloat)w/(GLfloat)h, 50*(GLfloat)w/(GLfloat)h, -50, 50, -100.0, 100.0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB);
    // Especifica as dimensões da janela de exibição
    glutInitWindowSize (500, 500); 
    // Especifica a posição inicial da janela de exibição
    glutInitWindowPosition (100, 100);
    // Cria a janela de exibição
    win_id = glutCreateWindow ("Visualizador de Modelos .obj");
    // Chama a função init
    init ();
    
    // Carrega modelo se fornecido como argumento
    if (argc > 1) {
        if (loadOBJ(argv[1])) {
            printf("Modelo %s carregado com sucesso!\n", argv[1]);
        }
    }
    
    printf("\n=== CONTROLES ===\n");
    printf("R/r: Cor vermelha\n");
    printf("G/g: Cor verde\n");
    printf("B/b: Cor azul\n");
    printf("L/l: Carregar arquivo .obj\n");
    printf("ESC: Sair\n");
    printf("Seta para cima: Tela cheia\n");
    printf("Seta para baixo: Janela 640x480\n");
    printf("Clique esquerdo: Cor aleatória\n\n");
    
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(myKeyboardSpecial);
    glutMouseFunc(myMouse);
    glutMainLoop();
    return 0;
}