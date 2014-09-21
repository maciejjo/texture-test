#include "mesh.h"

struct mesh *mesh_create(const char *fn) {

mesh *m = (mesh *) malloc(sizeof(struct mesh));
strcpy(m->filename, fn);
return m;

}

int mesh_load_mesh(struct mesh *mesh) {

  const struct aiScene *scene = aiImportFile(mesh->filename, aiProcess_Triangulate);
  if(!scene) {
    fprintf(stderr, "ERROR: reading mesh %s", mesh->filename);
    return -1;
  }

  const struct aiMesh *mesh_ai = scene->mMeshes[0];
  printf("%i vertices in mesh[0]\n", mesh_ai->mNumVertices);
  mesh->v_no = mesh_ai->mNumVertices;
  mesh->i_no = mesh_ai->mNumFaces;

  glGenVertexArrays(1, &mesh->vao);
  glBindVertexArray(mesh->vao);

  GLfloat *points = NULL;
  GLfloat *normals = NULL;
  GLfloat *texcoords = NULL;

	if(mesh_ai->mVertices != NULL && mesh_ai->mNumVertices > 0) {
    points = (GLfloat *) malloc (mesh->v_no * 3 * sizeof(GLfloat));
    for(unsigned int i = 0; i < mesh->v_no; i++) {
      const struct aiVector3D *vp = &(mesh_ai->mVertices[i]);
      points[i * 3] = (GLfloat) vp->x;
      points[i * 3 + 1] = (GLfloat) vp->y;
      points[i * 3 + 2] = (GLfloat) vp->z;
    }
  }

  if(mesh_ai->mNormals != NULL && mesh_ai->mNumVertices > 0) {
    normals = (GLfloat *) malloc (mesh->v_no * 3 * sizeof(GLfloat));
    for(unsigned int i = 0; i < mesh->v_no; i++) {
      const struct aiVector3D *vn = &(mesh_ai->mNormals[i]);
      normals[i * 3] = (GLfloat) vn->x;
      normals[i * 3 + 1] = (GLfloat) vn->y;
      normals[i * 3 + 2] = (GLfloat) vn->z;
    }
  }

  if(mesh_ai->mTextureCoords[0] != NULL && mesh_ai->mNumVertices > 0) {
    texcoords = (GLfloat *) malloc (mesh->v_no * 2 * sizeof(GLfloat));
    for(unsigned int i = 0; i < mesh->v_no; i++) {
      const struct aiVector3D *vt = &(mesh_ai->mTextureCoords[0][i]);
      texcoords[i * 2] = (GLfloat) vt->x;
      texcoords[i * 2 + 1] = (GLfloat) vt->y;
      printf("%d tex: %f, %f\n", i, vt->x, vt->y);

    }
  }

	if(mesh_ai->mVertices != NULL && mesh_ai->mNumVertices > 0) {
    printf("Loading position...\n");
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * mesh->v_no * sizeof(GLfloat),
        points, GL_STATIC_DRAW);
    GLint posAttrib = glGetAttribLocation(mesh->shader->id, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
    free(points);
  }

  if(mesh_ai->mNormals != NULL && mesh_ai->mNumVertices > 0) {
    printf("Loading normals...\n");
    GLuint vbo;
    glGenBuffers(1, &vbo);
    //glGenBuffers(1, &mesh->vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * mesh->v_no * sizeof(GLuint),
        points, GL_STATIC_DRAW);
    GLint normAttrib = glGetAttribLocation(mesh->shader->id, "normal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normAttrib);
    free(normals);
  }

  if(mesh_ai->mTextureCoords[0] != NULL && mesh_ai->mNumVertices > 0) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    printf("Loading texcoords...\n");
    //glGenBuffers(1, &mesh->vbo_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * mesh->v_no, points, GL_STATIC_DRAW);
    GLint texcoordAttrib = glGetAttribLocation(mesh->shader->id, "texcoord");
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texcoordAttrib);
    free(texcoords);
  }

  aiReleaseImport(scene);
  printf("Mesh loaded\n");

  return 0;
}

void mesh_load_shader(mesh *mesh, shader *shader) {

  mesh->shader = shader;

}

void mesh_load_texture(mesh *m, const char *fn) {

  printf("Loading texture...\n");

  glBindVertexArray(m->vao);

  GLuint tex;

  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  int width, height;

  unsigned char *img = SOIL_load_image(fn, &width, &height, 0, SOIL_LOAD_RGB);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

  SOIL_free_image_data(img);

  printf("Texture loaded\n");



}
