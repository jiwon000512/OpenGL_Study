#include "Mesh.h"

void Mesh::gen_gl_buffers()
{
    glGenBuffers(1, &position_buffer_);
    glGenBuffers(1, &color_buffer_);
    glGenBuffers(1, &normal_buffer_);
}

void Mesh::update_tv_indices()
{
    // triangle-vertex indices
    tv_indices_.clear();
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        aiFace &ai_face = pmesh_->mFaces[i];
        assert(ai_face.mNumIndices >= 3);

        // convert a polygon to a triangle fan
        for (unsigned int idx = 0; idx < ai_face.mNumIndices - 2; ++idx)
        {
            tv_indices_.push_back(ai_face.mIndices[0]);
            tv_indices_.push_back(ai_face.mIndices[idx + 1]);
            tv_indices_.push_back(ai_face.mIndices[idx + 2]);
        }
    }
}

void Mesh::set_gl_position_buffer_()
{
    assert(pmesh_->HasPositions());

    std::vector<glm::vec3> tv_positions; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_positions
    const std::vector<unsigned int> &tv_indices = tv_indices_;
    const aiVector3D *positions = pmesh_->mVertices;
    tv_positions.resize(tv_indices.size());
    for (std::size_t i = 0; i < tv_indices.size(); i += 3)
    {
        aiVector3D p = positions[tv_indices[i]];
        aiVector3D q = positions[tv_indices[i + 1]];
        aiVector3D r = positions[tv_indices[i + 2]];

        // Set the triangle-vertex positions
        tv_positions[i] = glm::vec3(p.x, p.y, p.z);
        tv_positions[i + 1] = glm::vec3(q.x, q.y, q.z);
        tv_positions[i + 2] = glm::vec3(r.x, r.y, r.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_positions.size(), &tv_positions[0], GL_STATIC_DRAW);
}

void Mesh::set_gl_color_buffer_(unsigned int cs_idx)
{
    assert(pmesh_->HasVertexColors(cs_idx));

    std::vector<glm::vec3> tv_colors; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_colors
    const aiColor4D *colors = pmesh_->mColors[cs_idx];
    const std::vector<unsigned int> &tv_indices = tv_indices_;

    // Resize the tv_colors vector to accommodate all triangle-vertex colors
    tv_colors.resize(tv_indices.size());

    // Set the tv_colors for each triangle
    for (std::size_t i = 0; i < tv_indices.size(); i += 3)
    {
        aiColor4D p = colors[tv_indices[i]];
        aiColor4D q = colors[tv_indices[i + 1]];
        aiColor4D r = colors[tv_indices[i + 2]];

        tv_colors[i] = glm::vec3(p.r, p.g, p.b);
        tv_colors[i + 1] = glm::vec3(q.r, q.g, q.b);
        tv_colors[i + 2] = glm::vec3(r.r, r.g, r.b);
    }

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_colors.size(), &tv_colors[0], GL_STATIC_DRAW);

    is_color_ = true;
}

void Mesh::set_gl_normal_buffer_(ShadingType shading_type)
{
    std::vector<glm::vec3> tv_flat_normals;   // per triangle-vertex flat normal (size = 3 x #triangles)
    std::vector<glm::vec3> tv_smooth_normals; // per triangle-vertex smooth normal (size = 3 x #triangles)
    std::vector<glm::vec3> v_smooth_normals;  // per-vertex 3D normal (size = #vertices)

    // init normals
    v_smooth_normals.resize(pmesh_->mNumVertices);
    for (std::size_t i = 0; i < v_smooth_normals.size(); ++i)
        v_smooth_normals[i] = glm::vec3(0.0f, 0.0f, 0.0f);

    // TODO: compute per-triangle normal &
    //       add_up to tv_flat_normals & v_smooth_normals
    tv_smooth_normals.resize(tv_indices_.size());
    for (std::size_t i = 0; i < tv_indices_.size(); i += 3)
    {
        unsigned int idx1 = tv_indices_[i];
        unsigned int idx2 = tv_indices_[i + 1];
        unsigned int idx3 = tv_indices_[i + 2];

        glm::vec3 v1 = glm::vec3(pmesh_->mVertices[idx1].x, pmesh_->mVertices[idx1].y, pmesh_->mVertices[idx1].z);
        glm::vec3 v2 = glm::vec3(pmesh_->mVertices[idx2].x, pmesh_->mVertices[idx2].y, pmesh_->mVertices[idx2].z);
        glm::vec3 v3 = glm::vec3(pmesh_->mVertices[idx3].x, pmesh_->mVertices[idx3].y, pmesh_->mVertices[idx3].z);

        glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);

        
        tv_flat_normals.push_back(normal);
        tv_flat_normals.push_back(normal);
        tv_flat_normals.push_back(normal);

        v_smooth_normals[idx1] += normal;
        v_smooth_normals[idx2] += normal;
        v_smooth_normals[idx3] += normal;
    }

    // normalize v_smooth_normals
    for (std::size_t i = 0; i < v_smooth_normals.size(); ++i)
        v_smooth_normals[i] = glm::normalize(v_smooth_normals[i]);

    // if pmesh_ has per-vertex normals, then just use them.
    if (pmesh_->HasNormals())
        memcpy(&v_smooth_normals[0], &pmesh_->mNormals[0], sizeof(pmesh_->mNormals[0]) * pmesh_->mNumVertices);

    // TODO: set tv_smooth_normals from v_smooth_normals
    tv_smooth_normals.resize(tv_indices_.size());

    for (std::size_t i = 0; i < tv_indices_.size(); i += 3)
    {
        unsigned int idx1 = tv_indices_[i];
        unsigned int idx2 = tv_indices_[i + 1];
        unsigned int idx3 = tv_indices_[i + 2];

        tv_smooth_normals[i] = v_smooth_normals[idx1];
        tv_smooth_normals[i + 1] = v_smooth_normals[idx2];
        tv_smooth_normals[i + 2] = v_smooth_normals[idx3];
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_smooth_normals.size(), &tv_smooth_normals[0], GL_STATIC_DRAW);
    }
    else // if (shading_type == kFlat)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_flat_normals.size(), &tv_flat_normals[0], GL_STATIC_DRAW);
    }
}

void Mesh::set_gl_buffers(ShadingType shading_type)
{
    set_gl_position_buffer_();
    if (pmesh_->HasVertexColors(0))
        set_gl_color_buffer_(0);
    set_gl_normal_buffer_(shading_type);
}

void Mesh::draw(int loc_a_position, int loc_a_normal)
{
    // TODO : draw a triangular mesh
    //          glBindBuffer() with position_buffer_
    //          glEnableVertexAttribArray() for loc_a_position
    //          glVertexAttribPointer() by reusing GPU data in loc_a_position
    //
    //          glBindBuffer() with normal_buffer_
    //          glEnableVertexAttribArray() for loc_a_normal
    //          glVertexAttribPointer() by reusing GPU data in loc_a_normal
    //
    //          glDrawArrays(GL_TRIANGLES, ...)
    //
    //          glDisableVertexAttribArray() for loc_a_position & loc_a_normal
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(loc_a_position);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(loc_a_normal);

    glDrawArrays(GL_TRIANGLES, 0, tv_indices_.size());

    glDisableVertexAttribArray(loc_a_position);

    glDisableVertexAttribArray(loc_a_normal);
}

void Mesh::print_info()
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