using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[RequireComponent(typeof(MeshFilter))]

public class MeshGenerator : MonoBehaviour
{
    [Tooltip("Grid X size")]
    public int xSize = 10;
    [Tooltip("Grid Z size")]
    public int zSize = 10;
    [Tooltip("X noise, Values between 0  and 1 please")]
    public float perlinX = .3f;
    [Tooltip("Z noise, Values between 0  and 1 please")]
    public float perlinZ = .2f;
    [Tooltip("Height multiplier")]
    public float perlinHeight = 2f;
    [Tooltip("Visual effect delay value")]
    public float delay = .01f;

    private Mesh mesh;
    private Vector3[] verticesArray;
    private int[] trianglesArray;

    // Start is called before the first frame update
    void Start()
    {
        mesh = new Mesh();
        this.GetComponent<MeshFilter>().mesh = mesh;

        CreateShape();
    }

    private void CreateShape()
    {
        CreateVertices();
        StartCoroutine(CreateTriangles());
    }

    private void UpdateMesh()
    {
        mesh.Clear(); //make sure the mesh is empty of any previous data

        //use our arrays
        mesh.vertices = verticesArray;
        mesh.triangles = trianglesArray;

        //normals recalculation, unity does it for ourselves
        mesh.RecalculateNormals(); //thats it
    }

    private void CreateVertices()
    {
        verticesArray = new Vector3[(xSize + 1) * (zSize + 1)];
        //loop over each of the vertices and assign a position on the gris, from left to right
        for (int i = 0, z = 0; z <= zSize; z++)
        {
            for (int x = 0; x <= xSize; x++)
            {
                //flat mesh for now
                verticesArray[i] = new Vector3(x, 0, z);
                i++;
            }
        }

        /*//create the vertices of the mesh
        verticesArray = new Vector3[]
        {
            new Vector3(0, 0, 0),
            new Vector3(0, 0, 1),
            new Vector3(1, 0, 0),
            new Vector3(1, 0, 1)
        };*/
    }

    private IEnumerator CreateTriangles()
    {
        trianglesArray = new int[xSize * zSize * 6];

        for (int triangIndex = 0, vertIndex = 0, z = 0; z < zSize; z++)
        {
            for (int x = 0; x < xSize; x++)
            {
                trianglesArray[triangIndex++] = vertIndex + 0;
                trianglesArray[triangIndex++] = vertIndex + xSize + 1;
                trianglesArray[triangIndex++] = vertIndex + 1;
                yield return new WaitForSeconds(delay);
                UpdateMesh(); //cool visual effect

                trianglesArray[triangIndex++] = vertIndex + 1;
                trianglesArray[triangIndex++] = vertIndex + xSize + 1;
                trianglesArray[triangIndex++] = vertIndex + xSize + 2;
                yield return new WaitForSeconds(delay);
                UpdateMesh();

                vertIndex++;
            }

            vertIndex++;
        }

        StartCoroutine(PerlinNoiseHeight()); //terrain

        /*//triangles must be created clock-wise direction, otherwise theyll not be seen
        trianglesArray = new int[]
        {
            0, 1, 2,
            1, 3, 2
        }; */
    }

    private IEnumerator PerlinNoiseHeight()
    {
        for (int i = 0; i < verticesArray.Length; i++)
        {
            float x = verticesArray[i].x;
            float z = verticesArray[i].z;
            float y = Mathf.PerlinNoise(x * perlinX, z * perlinZ) * perlinHeight;
            verticesArray[i].y = y;
            UpdateMesh();
            yield return new WaitForSeconds(delay);
        }
    }

    //visualize the vertices array
    private void OnDrawGizmos()
    {
        if (verticesArray != null)
        {
            for (int i = 0; i < verticesArray.Length; i++)
            {
                Gizmos.DrawSphere(verticesArray[i], .1f);
            }
        }
    }
}
