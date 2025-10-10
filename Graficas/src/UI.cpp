/**
 * @file    UI.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/10/2025
 */

/**
 * @include
 */
#include "UI.h"
#include "Prop.h"
#include "Actor.h"
#include "Model.h"
#include "Camera.h"
#include "Configs.h"
#include "SceneGraph.h"
#include "GraphicsAPI.h"
#include "WindowsFileDialogs.h"
#include "App.h"

#include <imgui.h>
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>

UI::UI() {
  m_rootFlags = ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_SpanFullWidth;

  m_treeSelectableFlags = m_rootFlags | ImGuiTreeNodeFlags_Selected;
  
  m_leafFlags = m_treeSelectableFlags |= ImGuiTreeNodeFlags_Leaf
                | ImGuiTreeNodeFlags_Bullet
                | ImGuiTreeNodeFlags_NoTreePushOnOpen;
}

UI::~UI() {
  destroy();
}

void UI::init(void* inHandle) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.DisplaySize = ImVec2(g_configs().s_windowSize.x, 
                          g_configs().s_windowSize.y);
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGuiStyle& style = ImGui::GetStyle();
  if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(inHandle);
  ImGui_ImplDX11_Init((ID3D11Device*)g_graphicsAPI().getDevice(),
                      (ID3D11DeviceContext*)g_graphicsAPI().getDeviceContext());

  //Change font path to engine settings
  io.Fonts->AddFontFromFileTTF(g_configs().s_defaultFontPath.string().c_str(), 15.0f);
}

void 
UI::render() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();

  renderUI();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
}

void 
UI::destroy() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

void 
UI::renderUI() {
auto& gapi = g_graphicsAPI();
  auto& shadMan = g_shaderManager();
  auto& texMan = g_textureManager();
  auto& sg = g_sceneGraph();

  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

  ImGui::BeginMainMenuBar(); {
    if(ImGui::BeginMenu("Options")) {
      if(ImGui::MenuItem("VSync", NULL, &g_configs().s_vsync)) {}
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if(ImGui::Begin("Menu" /*, NULL, ImGuiWindowFlags_NoMove */ )) {
    
    if(ImGui::CollapsingHeader("IO")) {
      if(ImGui::Button("Write model bin")) {
        //g_app().m_pDinoActor->m_model.exportToFile("Models/rex_norm.bin");
        //TODO: Change to selected model
      }
      //ImGui::SameLine();
      if(ImGui::Button("Load model from Bin")) {
        
      }
    }
    //Cameras
    if(ImGui::CollapsingHeader("CameraSettings")) {
      auto tmpMainCamera = sg.m_editorCamera;
      
      String tmpX = toString(tmpMainCamera->getPosition().x);
      String tmpY = toString(tmpMainCamera->getPosition().y);
      String tmpZ = toString(tmpMainCamera->getPosition().z);

      ImGui::Text("Camera position");
      ImGui::SameLine();
      ImGui::TextColored({0.91f, 0.07f, 0.14f, 1.f}, tmpX.substr(0, tmpX.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.05f, 0.76f, 0.26f, 1.f}, tmpY.substr(0, tmpY.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.f,   0.48f, 0.8f,  1.f}, tmpZ.substr(0, tmpZ.find(".") + 3).c_str());

      tmpX = toString(Radians(tmpMainCamera->m_YPR.x).getDegrees());
      tmpY = toString(Radians(tmpMainCamera->m_YPR.y).getDegrees());
      tmpZ = toString(Radians(tmpMainCamera->m_YPR.z).getDegrees());

      ImGui::Text("Camera rotation");
      ImGui::SameLine();
      ImGui::TextColored({0.91f, 0.07f, 0.14f, 1.f}, tmpX.substr(0, tmpX.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.05f, 0.76f, 0.26f, 1.f}, tmpY.substr(0, tmpY.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.f,   0.48f, 0.8f,  1.f}, tmpZ.substr(0, tmpZ.find(".") + 3).c_str());

      ImGui::SliderFloat("Camera speed", &tmpMainCamera->m_speed, 1.f, 400.f);
      ImGui::Separator();

      ImGui::SliderFloat("Camera Near", &tmpMainCamera->minZ, 0.001, 0.9);
      ImGui::SliderFloat("Camera Far", &tmpMainCamera->maxZ, 100, 10000);
      ImGui::SliderFloat("Camera FOV", &tmpMainCamera->halfFov, 0.1, 1.54);

    }
    ImGui::Separator(); // Textures
    if(ImGui::CollapsingHeader("Textures")) {
      void* tmpImage;
      
      //Positions
      tmpImage = g_app().gbuffer[0]->m_pSRV;
      ImGui::Text("Position Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Normals
      tmpImage = g_app().gbuffer[1]->m_pSRV;
      ImGui::Text("Normals Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Albedos
      tmpImage = g_app().gbuffer[2]->m_pSRV;
      ImGui::Text("Albedo Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Shadow map
      tmpImage = g_app().m_dsShadowMap->m_pSRV;
      ImGui::Text("Shadow Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //
    }
    ImGui::Separator(); // Light
    if(ImGui::CollapsingHeader("Lights")) {
      //ImGui::DragFloat3("Light Direction", &g_WVP.viewPos.x);
      ImGui::ColorEdit3("Light Color", &g_app().m_WVP.lightColor.x);
      g_shaderManager().setConstantValues(g_app().m_WVP);
      ImGui::Text("Light Direction: %.2f, %.2f, %.2f", 
                  g_app().m_WVP.viewPos.x, 
                  g_app().m_WVP.viewPos.y, 
                  g_app().m_WVP.viewPos.z);

      ////////////////////////////////////////////////////////////////////////////////////////////

      auto tmpActors = g_sceneGraph().getActorsByType(ActorType::kLight);
      SPtr<LightProp> tmpLight = static_pointer_cast<LightProp>(tmpActors[0]);

      ImGui::Text("Shadow Camera position");
      ImGui::SameLine();
      ImGui::DragFloat3("Position", &tmpLight->m_shadowCamera->m_position.x);

      ImGui::DragFloat("Light intensity", &g_app().m_WVP.lightIntensity);
      ImGui::DragFloat("Light Radius", &g_app().m_WVP.lightRadius);

      String tmpX = toString(Radians(tmpLight->m_shadowCamera->m_YPR.x).getDegrees());
      String tmpY = toString(Radians(tmpLight->m_shadowCamera->m_YPR.y).getDegrees());
      String tmpZ = toString(Radians(tmpLight->m_shadowCamera->m_YPR.z).getDegrees());

      ImGui::Text("Shadow Camera rotation");
      ImGui::SameLine();
      ImGui::DragFloat3("YawPitchRoll", &tmpLight->m_shadowCamera->m_YPR.x);
    }
    ImGui::Separator(); // Scene Graph
    if(ImGui::CollapsingHeader("Scene Graph")) {
      ImGui::Text("Scene ID: %s", toString(sg.m_sceneID.getUID()).c_str());
      ImGui::Text("Scene Name: %s", sg.m_sceneName.c_str());
      ImGui::Text("Scene Graph Actors: %d", sg.m_numActors);
      ImGui::Text("Actor Selected: %s", sg.getSelectedActor()->getName().c_str());
      //Hierarchy list
      if(ImGui::BeginTable("Hierarchy List", 3, ImGuiTableFlags_Resizable | 
                                                ImGuiTableFlags_NoBordersInBody |
                                                ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_NoHide);
        ImGui::TableHeadersRow();

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        
        renderNode(static_pointer_cast<Actor>(sg.getRoot()));
        

        ImGui::EndTable();
      }

    }
    ImGui::Separator(); //Materials in model
    if(ImGui::CollapsingHeader("Materials From a model")) {
      if(sg.getSelectedActor()->m_type != ActorType::kProp) {
        ImGui::Text("Select another Actor");
      }
      else {
        auto& tmpProp = static_pointer_cast<Prop>(sg.getSelectedActor())->m_model;

        ImGui::Text("Actor Selected: %s", sg.getSelectedActor()->getName().c_str());
        ImGui::Text("Actor UID: %s", toString(sg.getSelectedActor()->getID()).c_str());
        
        auto& tmpMat = tmpProp.m_meshes[m_meshIndex].meshMaterial;
        ImGui::Text("Material name: %s", tmpMat.m_materialName.c_str());


        ImGui::SliderInt(tmpProp.m_meshes[m_meshIndex].meshName.c_str(), 
                         &m_meshIndex, 
                         0, 
                         tmpProp.m_meshes.size() - 1);
        ImGui::ColorEdit3("Albedo Color",    &tmpMat.m_albedoColor.x);
        ImGui::ColorEdit3("Emissive Color",  &tmpMat.m_emissiveColor.x);
        ImGui::SliderFloat("Metallic value", &tmpMat.m_metallic, 0, 1);
        ImGui::SliderFloat("Rougness value", &tmpMat.m_roughness, 0, 1);
        ImGui::SliderFloat("Specular value", &tmpMat.m_specular, 0, 1);
        
        ImGui::Separator();
        //Albedo
        ImVec2 imgSize = {50.f, 50.f};
        ImTextureRef texturesID[5];
        TextureRef texRefID[5];

        texRefID[0] = tmpMat.getTextureRef(TEXTURE_TYPE::kAlbedo);
        texRefID[1] = tmpMat.getTextureRef(TEXTURE_TYPE::kNormal);
        texRefID[2] = tmpMat.getTextureRef(TEXTURE_TYPE::kMetallic);
        texRefID[3] = tmpMat.getTextureRef(TEXTURE_TYPE::kRoughness);
        texRefID[4] = tmpMat.getTextureRef(TEXTURE_TYPE::kEmissive);

        texturesID[0] = texMan.getTexture(texRefID[0])->m_pSRV;
        texturesID[1] = texMan.getTexture(texRefID[1])->m_pSRV;
        texturesID[2] = texMan.getTexture(texRefID[2])->m_pSRV;
        texturesID[3] = texMan.getTexture(texRefID[3])->m_pSRV;
        texturesID[4] = texMan.getTexture(texRefID[4])->m_pSRV;
        
        for(int32 i = 0; i < 5; ++i) {
          ImGui::PushID(i);
          if(ImGui::ImageButton("Texture" + i, texturesID[i], imgSize)) {
            //tmpMat.changeTexture(, TEXTURE_TYPE::kAlbedo);
          }
          //D&D Source
          if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("TexChange", &texRefID[i], sizeof(TextureRef));
            ImGui::Text("Copying texture: %s", toString(texRefID[i].id).c_str());
            ImGui::EndDragDropSource();
          }
          //D&D Target
          if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexChange")) {
              IM_ASSERT(payload->DataSize == sizeof(TextureRef));
              TextureRef payload_n = *(const TextureRef*)payload->Data;
              switch(i) {
                case 0:
                  tmpMat.changeTexture(payload_n, TEXTURE_TYPE::kAlbedo);
                  break;
                case 1:
                  tmpMat.changeTexture(payload_n, TEXTURE_TYPE::kNormal);
                  break;
                case 2:
                  tmpMat.changeTexture(payload_n, TEXTURE_TYPE::kMetallic);
                  break;
                case 3:
                  tmpMat.changeTexture(payload_n, TEXTURE_TYPE::kRoughness);
                  break;
                case 4:
                  tmpMat.changeTexture(payload_n, TEXTURE_TYPE::kEmissive);
                  break;
                default:
                  ConsoleOut << "Error: Out of bounds texture type." << ConsoleLine;
                  __debugbreak();
                  break;
              }
              tmpProp.m_meshes[m_meshIndex].meshMaterial = tmpMat;
              
            }
            ImGui::EndDragDropTarget();
          }
          ImGui::PopID();
          ImGui::SameLine();
        }
      }
    }
    ImGui::Separator(); // Texture Manager
    if(ImGui::CollapsingHeader("Texture Manager")) {
      ImGui::Text("Texture Manager / Loaded Textures");
      ImGui::Text("Total Loaded Textures: %d", texMan.m_textures.size());
      if(ImGui::Button("Add Texture", {256, ImGui::GetFontSize() * 1.5f})) {
        
        Path tmpPath = WindowsFileDialogs::openFileDialog(m_handle, WindowsFileDialogs::m_fileFiltersImage);
        if(!tmpPath.empty()) {
          texMan.loadTexture(tmpPath);
        }
      }

      ImGui::Separator();
      ImVec2 imgSize = {50.f, 50.f};
      auto size = texMan.m_textures.size();
      
      int32 i = 0;
      for(auto& tex : texMan.m_textures) {
        ImGui::PushID(i);
        if((i % 3) != 0) {
          ImGui::SameLine();
        }
        if(ImGui::ImageButton("xd2", tex.second->m_pSRV, imgSize)) {

        }
        //D&D Source
        if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
          ImGui::SetDragDropPayload("TexChange", &tex.first, sizeof(TextureRef));
          ImGui::Text("Copying texture: %s", toString(tex.first.id).c_str());
          ImGui::EndDragDropSource();
        }
        
        ImGui::PopID();
        //ImGui::SameLine();
        ++i;
      }
    }
    ImGui::Separator(); // Debug
    if(ImGui::CollapsingHeader("Debug")) {
      ImGui::Separator();
      if(sg.getSelectedActor()->m_type != ActorType::kProp) {
        ImGui::Text("Select another Actor");
      }
      else {
        Model& tmpModel = static_pointer_cast<Prop>(sg.getSelectedActor())->m_model;
        
        ImGui::SliderInt("Mesh draw limit", &tmpModel.m_debugMesh, -1, tmpModel.m_meshes.size()-1);
      }
      ImGui::Separator();
    }
    ImGui::Separator(); // Delta Time
    {
      //ImGui::Text("Delta Time: %.3f ms/frame", g_app().m_appTime.getTime());
      ImGui::Text("FPS: %.1f", 1.f / g_app().m_appTime.getTime());

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    
  } ImGui::End();

  if(ImGui::Begin("Viewport")) {
    auto tmpSize = ImGui::GetContentRegionAvail();
    /*g_viewportSize = Vector2(tmpSize.x, tmpSize.y);
    if(g_prevViewportSize != g_viewportSize) {
      resizeTextures();
    }*/
    
    ImGui::Image(g_app().m_renderPassRT->m_pSRV, tmpSize);
  } ImGui::End();
  
  ImGui::ShowDemoWindow();
}

void 
UI::renderNode(SPtr<Actor> inNode) {
  auto& sg = g_sceneGraph();

  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  bool tmpOpenNode = false;

  if(0 < inNode->m_children.size()) {  //It has a child
    tmpOpenNode = ImGui::TreeNodeEx(inNode->getName().c_str(), m_rootFlags);
  }
  else {  //It hasn't childs
    ImGui::TreeNodeEx(inNode->getName().c_str(), m_leafFlags);
  }

  if(ImGui::IsItemClicked()) {
    sg.setSelectedActor(inNode);
  }

  //Type of actor
  ImGui::TableNextColumn();
  ImGui::Text(inNode->getTypeAsString().c_str());

  //Actor active
  ImGui::TableNextColumn();
  //TODO: Fix the functionality of this checkbox.
  ImGui::Checkbox("", &inNode->isActive);

  if(0 < inNode->m_children.size() && tmpOpenNode) {
    for(auto& nodes : inNode->m_children) {
      renderNode(static_pointer_cast<Actor>(nodes));
    }
    ImGui::TreePop();
  }
}

UI&
g_ui() {
  return UI::instance();
}