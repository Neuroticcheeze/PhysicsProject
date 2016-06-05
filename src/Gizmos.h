#pragma once

#define GLM_SWIZZLE 
#include <glm\mat4x4.hpp>
#include <glm/glm.hpp>
#include "gl_core_4_4.h"
#include <vector>
using std::vector;

class Gizmos {
private:

	struct GizmoVertex;

public:

	struct GizmoMesh
	{
		friend class Gizmos;

	private:

		glm::vec4 m_colour;
		glm::vec3 m_center;
		glm::mat4 m_matrix;

		float m_lineWidth;

		vector<float> m_vertices;
		vector<unsigned int> m_indices;

		unsigned int m_vbo, m_vao, m_ibo;

	public:
		//Vertex structure must be exactly x/y/z/r/g/b/a. Note, r/g/b/a is not used.Colour is set on Gizmos::addMesh(...)
		void Create(const vector<float> & p_vertices, const vector<unsigned int> & p_indices)
		{
			m_vertices = p_vertices;
			m_indices = p_indices;

			glGenVertexArrays(1, &m_vao); // Create our Vertex Array Object  
			glBindVertexArray(m_vao); // Bind our Vertex Array Object so we can use it  

			glGenBuffers(1, &m_vbo); // Generate our Vertex Buffer Object  
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // Bind our Vertex Buffer Object  
			glBufferData(GL_ARRAY_BUFFER, p_vertices.size() * sizeof(float), p_vertices.data(), GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW  
																													   //We could have actually made one big IBO for both the quad and triangle.
			glGenBuffers(1, &m_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof(unsigned int), p_indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), ((char*)0) + 16);

			glBindVertexArray(0);
		}

		void Destroy()
		{
			m_vertices.clear();
			m_indices.clear();

			glDeleteBuffers(1, &m_vbo);
			glDeleteBuffers(1, &m_ibo);
			glDeleteVertexArrays(1, &m_vao);
		}

		inline vector<float> GetVertices() const { return m_vertices; }
		inline vector<unsigned int> GetIndices() const { return m_indices; }
	};

	static void		create(unsigned int a_maxPoints = 0xffff, unsigned int a_maxMeshes = 0xffff, unsigned int a_maxLines = 0xffff, unsigned int a_maxTris = 0xffff,
						   unsigned int a_max2DLines = 0xff, unsigned int a_max2DTris = 0xff);
	static void		destroy();

	// removes all Gizmos
	static void		clear();

	// draws current Gizmo buffers, either using a combined (projection * view) matrix, or separate matrices
	static void		draw(const glm::mat4& a_projectionView);
	static void		draw(const glm::mat4& a_projection, const glm::mat4& a_view);
	
	// the projection matrix here should ideally be orthographic with a near of -1 and far of 1
	static void		draw2D(const glm::mat4& a_projection);

	// Adds a single debug mesh
	static void		addMesh(GizmoMesh a_mesh, const glm::vec3& a_center,
							const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr, const float & p_lineWidth = 1.0F);

	// Adds a single debug point. Transparency is not enabled for points. Set point size with glPointSize(...)
	static void		addPoint(const glm::vec3& a_pos, const glm::vec4& a_colour);

	// Adds a single debug line
	static void		addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, 
							const glm::vec4& a_colour);

	// Adds a single debug line
	static void		addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, 
							const glm::vec4& a_colour0, const glm::vec4& a_colour1);

	// Adds a triangle.
	static void		addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour);

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	static void		addTransform(const glm::mat4& a_transform, float a_fScale = 1.0f);
	
	// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
	static void		addAABB(const glm::vec3& a_center, const glm::vec3& a_extents, 
							const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);

	// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
	static void		addAABBFilled(const glm::vec3& a_center, const glm::vec3& a_extents, 
								  const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
	static void		addCylinderFilled(const glm::vec3& a_center, float a_radius, float a_fHalfLength,
									  unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer and inner line is drawn.
	static void		addRing(const glm::vec3& a_center, float a_innerRadius, float a_outerRadius,
							unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a double-sided disk in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addDisk(const glm::vec3& a_center, float a_radius,
							unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds an arc, around the Y-axis
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addArc(const glm::vec3& a_center, float a_rotation,
						   float a_radius, float a_halfAngle,
						   unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addArcRing(const glm::vec3& a_center, float a_rotation, 
							   float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
							   unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
	static void		addSphere(const glm::vec3& a_center, float a_radius, int a_rows, int a_columns, const glm::vec4& a_fillColour, 
							  const glm::mat4* a_transform = nullptr, float a_longMin = 0.f, float a_longMax = 360, 
							  float a_latMin = -90, float a_latMax = 90 );

	// Adds a single Hermite spline curve
	static void		addHermiteSpline(const glm::vec3& a_start, const glm::vec3& a_end,
									 const glm::vec3& a_tangentStart, const glm::vec3& a_tangentEnd, unsigned int a_segments, const glm::vec4& a_colour);

	// 2-dimensional gizmos
	static void		add2DLine(const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec4& a_colour);
	static void		add2DLine(const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec4& a_colour0, const glm::vec4& a_colour1);	
	static void		add2DTri(const glm::vec2& a_0, const glm::vec2& a_1, const glm::vec2& a_2, const glm::vec4& a_colour);	
	static void		add2DAABB(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);	
	static void		add2DAABBFilled(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);	
	static void		add2DCircle(const glm::vec2& a_center, float a_radius, unsigned int a_segments, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);
	
private:

	Gizmos(unsigned int a_maxPoints, unsigned int a_maxMeshes, unsigned int a_maxLines, unsigned int a_maxTris,
		   unsigned int a_max2DLines, unsigned int a_max2DTris);
	~Gizmos();


	struct GizmoVertex {
		float x, y, z, w;
		float r, g, b, a;
	};

	struct GizmoLine {
		GizmoVertex v0;
		GizmoVertex v1;
	};

	struct GizmoTri {
		GizmoVertex v0;
		GizmoVertex v1;
		GizmoVertex v2;
	};

	unsigned int	m_shader;

	// mesh data
	unsigned int	m_maxMeshes;
	unsigned int	m_meshCount;
	GizmoMesh*		m_meshes;

	// point data
	unsigned int	m_maxPoints;
	unsigned int	m_pointCount;
	GizmoVertex*	m_points;

	unsigned int	m_pointVAO;
	unsigned int 	m_pointVBO;

	// line data
	unsigned int	m_maxLines;
	unsigned int	m_lineCount;
	GizmoLine*		m_lines;

	unsigned int	m_lineVAO;
	unsigned int 	m_lineVBO;

	// triangle data
	unsigned int	m_maxTris;
	unsigned int	m_triCount;
	GizmoTri*		m_tris;

	unsigned int	m_triVAO;
	unsigned int 	m_triVBO;
	
	unsigned int	m_transparentTriCount;
	GizmoTri*		m_transparentTris;

	unsigned int	m_transparentTriVAO;
	unsigned int 	m_transparentTriVBO;
	
	// 2D line data
	unsigned int	m_max2DLines;
	unsigned int	m_2DlineCount;
	GizmoLine*		m_2Dlines;

	unsigned int	m_2DlineVAO;
	unsigned int 	m_2DlineVBO;

	// 2D triangle data
	unsigned int	m_max2DTris;
	unsigned int	m_2DtriCount;
	GizmoTri*		m_2Dtris;

	unsigned int	m_2DtriVAO;
	unsigned int 	m_2DtriVBO;

	static Gizmos*	sm_singleton;
};