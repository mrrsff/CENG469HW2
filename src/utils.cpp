#include "utils.h"

Mesh* ParseObjFile(const char* path, bool useTriangles, bool withNormals)
{
    std::vector<Vector3>* vertices = new std::vector<Vector3>();
    std::vector<Vector2>* textures = new std::vector<Vector2>();
    std::vector<Vector3>* normals = new std::vector<Vector3>();
    std::vector<Triangle>* triangles = new std::vector<Triangle>();
    std::vector<Quad>* quads = new std::vector<Quad>();
    std::fstream myfile;

    myfile.open(path, std::ios::in);

    if (!myfile.is_open())
    {
        std::cerr << "Unable to open file " << path << std::endl;
        return nullptr;
    }

    std::string line;
    while (getline(myfile, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            ss >> x >> y >> z;
            vertices->emplace_back(x, y, z);
        }
        else if (prefix == "vt")
        {
            float u, v;
            ss >> u >> v;
            textures->emplace_back(u, v);
        }
        else if (prefix == "vn" && withNormals)
        {
            float x, y, z;
            ss >> x >> y >> z;
            normals->emplace_back(x, y, z);
        }
        else if (prefix == "f")
        {
            std::vector<int> *vIndices,  *nIndices;
            vIndices = new std::vector<int>();
            nIndices = new std::vector<int>();
            std::string vertex;
            while (ss >> vertex)
            {
                std::replace(vertex.begin(), vertex.end(), '/', ' ');
                std::stringstream vertexStream(vertex);
                int v, n;
                vertexStream >> v >> n; // Assuming every vertex in a face has v//n format.
                // OBJ indices are 1-based, so we need to subtract 1.
                vIndices->push_back(v-1); 
                nIndices->push_back(n-1);
            }
            if(!withNormals)
            {
                normals->emplace_back(0.0f, 0.0f, 0.0f);
            }

            int v1[3] = {vIndices->at(0), vIndices->at(1), vIndices->at(2)};
            int n1[3] = {nIndices->at(0), nIndices->at(1), nIndices->at(2)};
            triangles->emplace_back(Triangle(v1, v1, n1));
        }
    }

    myfile.close();

    // Basic validation
    if (vertices->empty() || (triangles->empty() && quads->empty()))
    {
        std::cerr << "Invalid or empty OBJ file\n";
		return nullptr;
    }

    // Successful parsing, create and return the mesh
    if (useTriangles)
    {
        Mesh* mesh = new Mesh();
        mesh->vertices = vertices;
        mesh->normals = normals;
        mesh->textures = textures;
        mesh->triangles = triangles;
        mesh->UpdateMesh();
        return mesh;
    }
    else
    {
        Mesh* mesh = new Mesh();
        mesh->quadMesh = true;
        mesh->vertices = vertices;
        mesh->normals = normals;
        mesh->textures = textures;
        mesh->quads = quads;
        mesh->UpdateMesh();
        return mesh;
    }
}

void CheckGLError(const char* file, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error at " << file << ":" << line << " " << err << std::endl;
		assert(false);
	}
}

Quaternion utilsLookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Matrix4 lookMatrix = glm::lookAt(position, target, up);
	Quaternion rotation = glm::quat_cast(lookMatrix);
	return rotation;
}

Quaternion utilsFromAxisAngle(Vector3 axis, double angle)
{
	Quaternion rotation = glm::angleAxis(glm::radians((float)angle), axis);
	return rotation;
}

Vector3 utilsRotatePointAroundAxis(Vector3 point, Vector3 axis, double angle)
{
    Quaternion rotation = utilsFromAxisAngle(axis, angle);
    Vector3 rotatedPoint = rotation * point;
    return rotatedPoint;
}
