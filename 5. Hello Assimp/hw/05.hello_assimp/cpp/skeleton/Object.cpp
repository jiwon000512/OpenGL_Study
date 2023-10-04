#include "Object.h"

void Object::init_buffer_objects()
{
    // TODO
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, pmesh_->mNumVertices * sizeof(aiVector3D), pmesh_->mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, pmesh_->mNumVertices * sizeof(aiColor4D), pmesh_->mColors[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < pmesh_->mNumFaces; i++)
    {
        struct Face face;
        glGenBuffers(1, &face.index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, pmesh_->mFaces[i].mNumIndices * sizeof(GLuint), pmesh_->mFaces[i].mIndices, GL_STATIC_DRAW);
        face.num_indices = pmesh_->mFaces[i].mNumIndices;
        faces.push_back(face);
    }

}

glm::mat4 Object::get_model_matrix() const
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    mat_model = glm::translate(mat_model, vec_translate_);
    mat_model *= glm::mat4_cast(quat_rotate_);
    mat_model = glm::scale(mat_model, vec_scale_);

    return mat_model;
}

void Object::draw(int loc_a_position, int loc_a_color)
{
    // TODO
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glEnableVertexAttribArray(loc_a_color);
    glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    
    for (unsigned int i = 0; i < pmesh_->mNumFaces; i++)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces[i].index_buffer);
        glDrawElements(GL_TRIANGLES, faces[i].num_indices, GL_UNSIGNED_INT, (void*)0);
    }

    // 정점 attribute 배열 비활성화
    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_color);
}

void Object::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << pmesh_->mNumVertices << std::endl;
    for (unsigned int i = 0; i < pmesh_->mNumVertices; ++i)
    {
        aiVector3D vertex = pmesh_->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (pmesh_->mColors[0] != NULL)
        {
            aiColor4D color = pmesh_->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}