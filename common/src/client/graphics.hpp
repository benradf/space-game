/// \file graphics.hpp
/// \brief Handles client graphics.
/// \author Ben Radford 
/// \date 24th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP


#include <memory>
#include <vector>
#include <OGRE/Ogre.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <CEGUI/CEGUI.h>
//#include <CEGUI/CEGUILua.h>
#include <boost/shared_ptr.hpp>


class Input;
class Login;
class CEGUIInput;
class LocalController;
class NetworkInterface;


namespace gfx {


class MovableParticleSystem {
    public:
        MovableParticleSystem(const char* name, const char* system, 
            const Ogre::Vector3& offset, Ogre::SceneNode* node, 
            Ogre::SceneManager* sceneManager);
        ~MovableParticleSystem();

        void update(const Ogre::Vector3& velocity);
        Ogre::ParticleSystem& get();

        void setEnabled(bool enabled);
        void setVisible(bool visible);
        void setPaused(bool paused);

    private:
        Ogre::SceneManager* _sceneManager;
        Ogre::ParticleSystem* _system;
        Ogre::SceneNode* _node;

        std::vector<Ogre::Vector3> _dir;
        Ogre::Vector3 _offset;
};


class Camera {
    public:
        friend class Viewport;

        Camera(const char* name, Ogre::SceneManager* sceneManager);
        ~Camera();

        void setPosition(const Ogre::Vector3& pos);
        const Ogre::Vector3& getPosition() const;

        const Ogre::Matrix4& getViewMatrix() const;

        void lookAt(const Ogre::Vector3& pos);

    private:
        Camera(const Camera&);
        Camera& operator=(const Camera&);

        Ogre::SceneManager* _sceneManager;
        Ogre::Camera* _camera;
};


class ObjectOverlay {
    public:
        ObjectOverlay(Ogre::Overlay* overlay, const char* name);
        ~ObjectOverlay();

        void attachCamera(Camera& camera);
        void update(Ogre::MovableObject* object);

        void setColour(const Ogre::ColourValue& colour);
        void setText(const char* text);
        void setVisible(bool visible);

    private:
        void cleanUp();

        Ogre::OverlayContainer* _container;
        Ogre::OverlayElement* _text;
        Ogre::Overlay* _overlay;
        Camera* _camera;
};


class Entity {
    public:
        Entity(const char* name, const char* mesh, Ogre::SceneManager* sceneManager);
        ~Entity();

        void setPosition(const Ogre::Vector3& pos);
        const Ogre::Vector3& getPosition() const;

        void setOrientation(const Ogre::Quaternion& rot);
        const Ogre::Quaternion& getOrientation() const;

        void setMaterial(const char* material) const;

        MovableParticleSystem* attachParticleSystem(
            const char* system, const Ogre::Vector3& offset);

        void updateParticleSystems(const Ogre::Vector3& velocity);

        bool hasObjectOverlay() const;
        ObjectOverlay& getObjectOverlay();

        void attachObjectOverlay(std::unique_ptr<ObjectOverlay> overlay);
        void updateObjectOverlay();

    private:
        Entity(const Entity&);
        Entity& operator=(const Entity&);

        Ogre::SceneManager* _sceneManager;
        Ogre::SceneNode* _node;
        Ogre::Entity* _entity;
        std::string _name;

        std::vector<MovableParticleSystem*> _particleSystems;
        std::unique_ptr<ObjectOverlay> _objectOverlay;
};


class Backdrop {
    public:
        Backdrop(const char* name, const char* material, float scroll, 
            float depth, float size, Ogre::SceneManager* sceneManager);
        ~Backdrop();

        void update(const Ogre::Vector3& centre);

    private:
        float _scroll;
        float _depth;
        float _size;

        std::string _name;
        Ogre::MeshPtr _mesh;
        std::unique_ptr<Entity> _entity;
};


class GUI {
    public:
        GUI(Ogre::RenderWindow* window, Ogre::SceneManager* sceneManager, Input& input);
        ~GUI();

        void render();

        std::unique_ptr<class HUD> createHUD(NetworkInterface& network, 
            LocalController& localController);

        std::unique_ptr<Login> createLogin();

    private:
        std::unique_ptr<CEGUIInput> _ceguiInput;
        CEGUI::OgreRenderer* _ceguiRenderer;
        CEGUI::System* _ceguiSystem;
};


class Scene {
    public:
        Scene(boost::shared_ptr<Ogre::Root> root);
        ~Scene();

        std::unique_ptr<Camera> createCamera(const char* name);
        std::unique_ptr<Entity> createEntity(const char* name, const char* mesh);
        std::unique_ptr<ObjectOverlay> createObjectOverlay(const char* name);

        void setSkyPlane(const char* material, const Ogre::Vector3& normal, float dist);
        void addBackdrop(const char* material, float scroll, float depth, float size);

        void updateBackdropPositions(const Ogre::Vector3& centre);
        void setCEGUIRenderer(CEGUI::OgreRenderer* renderer);

    private:
        Scene(const Scene&);
        Scene& operator=(const Scene&);

        boost::shared_ptr<Ogre::Root> _root;
        Ogre::SceneManager* _sceneManager;
        Ogre::OverlaySystem* _overlaySystem;

        std::vector<Backdrop*> _backdrops;

        Ogre::Overlay* _objectOverlay;
};


class Viewport {
    public:
        Viewport(const char* title, boost::shared_ptr<Ogre::Root> root);
        ~Viewport();

        void update();

        Input& getInput();

        void attachCamera(Camera& camera);

        Ogre::RenderWindow* getRenderWindow();

        std::unique_ptr<GUI> createGUI();

    private:
        Viewport(const Viewport&);
        Viewport& operator=(const Viewport&);

        void updateAspectRatio();

        boost::shared_ptr<Ogre::Root> _root;
        std::unique_ptr<Input> _input;
        Ogre::RenderWindow* _window;
        Ogre::Viewport* _viewport;
        Ogre::Camera* _camera;
};


class GFXManager {
    public:
        GFXManager();
        ~GFXManager();

        void render();

        Viewport& getViewport();

        std::unique_ptr<Scene> createScene();
        std::unique_ptr<Viewport> createViewport();

    private:
        GFXManager(const GFXManager&);
        GFXManager& operator=(const GFXManager&);

        void initResources();

        boost::shared_ptr<Ogre::Root> _root;
        std::unique_ptr<Viewport> _viewport;
};


}  // namespace gfx


#endif  // GRAPHICS_HPP

