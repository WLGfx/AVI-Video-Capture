#include "windows.h"
#include "commctrl.h"
#include "DBP-entry.h"
#include "DBPro.hpp"

//EXPORTC void CreateBlankObject(int ObjectId, int VertexCount, int IndexCount, int Fvf, bool Zeroise)
//{
//    // Create the new object
//    DBPro::CreateNewObject(ObjectId, "", 1);
//
//    // Get a pointer to the object, then to the mesh for the first limb (Frame = limb)
//    sObject* Object = DBPro::GetObjectData(ObjectId);
//    sMesh* Mesh = Object->pFrame->pMesh;
//
//    // Populate the vertex & index arrays.
//    // It is valid to have an IndexCount of zero, for index-less meshes.
//    // IndexCount should be a multiple of 3 if present.
//    DBPro::SetupMeshFvfData( Mesh, Fvf, VertexCount, IndexCount );
//
//    // You can replace this bit with your own vertex setup.
//    // I'm just zeroising everything here.
//    if (Zeroise)
//    {
//        if (VertexCount)
//            ZeroMemory(Mesh->pVertexData, Mesh->dwFVFSize * VertexCount);
//        if (IndexCount)
//            ZeroMemory(Mesh->pIndices, sizeof(WORD) * IndexCount);
//    }
//    /*
//    // You can use the SetupStandardVertex function to fill out the vertices also.
//    // Note that this doesn't work properly if you have multiple texture coordinates, as the texture coords
//    // that aren't set will not be zeroised.
//    for (DWORD VertexId = 0; VertexId < VertexCount; ++VertexId)
//    {
//        DBPro::SetupStandardVertex(Mesh->dwFVF, Mesh->pVertexData, VertexId, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0xffffffff, 0.0, 0.0);
//    }
//    */
//
//    // Set up the mesh type
//    Mesh->iPrimitiveType = D3DPT_TRIANGLELIST;
//
//    // Set up the vertex count (for transforms)
//    Mesh->iDrawVertexCount = VertexCount;
//
//    // If there is an index count, then the primitive count = index count / 3
//    // otherwise, the primitive count = vertex count / 3
//    Mesh->iDrawPrimitives = (IndexCount ? IndexCount : VertexCount)  / 3;
//
//    // Finalise the object
//    // Setting a radius of zero causes the correct radius to be calculated from the mesh
//    DBPro::SetNewObjectFinalProperties(ObjectId, 0.0f);
//    DBPro::TextureObject(ObjectId, 0);
//}
