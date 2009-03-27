/// \file graphics.cpp
/// \brief Handles client graphics.
/// \author Ben Radford 
/// \date 24th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "graphics.hpp"
#include <core.hpp>


////////// Entity //////////

Entity::Entity(const char* name, const char* mesh, Ogre::SceneManager* sceneManager) :
    _sceneManager(sceneManager), _node(0), _entity(0), _name(name)
{
    Ogre::SceneNode* rootNode = _sceneManager->getRootSceneNode();
    if ((_entity = _sceneManager->createEntity(name, mesh)) == 0) 
        throw MemoryException("failed to create an entity");

    if ((_node = rootNode->createChildSceneNode(name)) == 0) {
        _sceneManager->destroyEntity(_entity);
        throw MemoryException("failed to create an entity");
    }

    _node->attachObject(_entity);
    _node->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
}

Entity::~Entity()
{
    _node->detachObject(_entity);
    _sceneManager->destroyEntity(_entity);
    _sceneManager->getRootSceneNode()->removeAndDestroyChild(_name);
}

void Entity::setPosition(const Ogre::Vector3& pos)
{
    _node->setPosition(pos);
}

const Ogre::Vector3& Entity::getPosition() const
{
    return _node->getPosition();
}

void Entity::setOrientation(const Ogre::Quaternion& rot)
{
    _node->setOrientation(rot);
}

const Ogre::Quaternion& Entity::getOrientation() const
{
    return _node->getOrientation();
}


////////// Camera //////////

Camera::Camera(const char* name, Ogre::SceneManager* sceneManager) :
    _camera(0), _sceneManager(sceneManager)
{
    _camera = _sceneManager->createCamera(name);
    _camera->setNearClipDistance(1.0f);
    _camera->setFarClipDistance(50000.0f);
}

Camera::~Camera()
{
    _sceneManager->destroyCamera(_camera);
}

void Camera::setPosition(const Ogre::Vector3& pos)
{
    _camera->setPosition(pos);
}

const Ogre::Vector3& Camera::getPosition() const
{
    return _camera->getPosition();
}

void Camera::lookAt(const Ogre::Vector3& pos)
{
    _camera->lookAt(pos);
}


////////// Scene //////////

Scene::Scene(boost::shared_ptr<Ogre::Root> root) :
    _root(root), _sceneManager(0)
{
    _sceneManager = _root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "ExteriorSceneManager");
    //_sceneManager->setDisplaySceneNodes(true);
}

Scene::~Scene()
{
    _root->destroySceneManager(_sceneManager);
}

std::auto_ptr<Camera> Scene::createCamera(const char* name)
{
    return std::auto_ptr<Camera>(new Camera(name, _sceneManager));
}

std::auto_ptr<Entity> Scene::createEntity(const char* name, const char* mesh)
{
    return std::auto_ptr<Entity>(new Entity(name, mesh, _sceneManager));
}

void Scene::setSkyPlane(const char* material, const Ogre::Vector3& normal, float dist)
{
    _sceneManager->setSkyPlane(true, Ogre::Plane(normal, Ogre::Real(dist)), material,
        Ogre::Real(500.0f), Ogre::Real(100.0f));
}


////////// Viewport //////////

Viewport::Viewport(const char* title, boost::shared_ptr<Ogre::Root> root) :
    _root(root), _window(0), _viewport(0), _camera(0)
{
    _window = _root->initialise(true, title);
}

Viewport::~Viewport()
{
    _window->removeAllViewports();
    _root->detachRenderTarget(_window);
}

void Viewport::update()
{
    _window->update();
}

void Viewport::attachCamera(Camera& camera)
{
    _window->removeAllViewports();
    _camera = camera._camera;
    _viewport = _window->addViewport(_camera);
    _window->update();

    updateAspectRatio();
}

void Viewport::updateAspectRatio()
{
    Ogre::Real width = _viewport->getActualWidth();
    Ogre::Real height = _viewport->getActualHeight();
    _camera->setAspectRatio(width / height);
}


////////// GFXManager //////////

GFXManager::GFXManager()
{
    _root.reset(new Ogre::Root());
    //_root->showConfigDialog();
    _root->restoreConfig();
    initResources();

    _viewport.reset(new Viewport("mmoclient", _root));

    
}

GFXManager::~GFXManager()
{

}

void GFXManager::render()
{
    _root->renderOneFrame();
}

Viewport& GFXManager::getViewport()
{
    return *_viewport;
}

std::auto_ptr<Scene> GFXManager::createScene()
{
    return std::auto_ptr<Scene>(new Scene(_root));
}

void GFXManager::initResources()
{
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator iter = cf.getSectionIterator();

    while (iter.hasMoreElements()) {
        Ogre::String secName = iter.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = iter.getNext();

        typedef Ogre::ConfigFile::SettingsMultiMap::iterator Iterator;
        for (Iterator i = settings->begin(); i != settings->end(); ++i) {
            Ogre::String typeName = i->first;
            Ogre::String archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().
                addResourceLocation(archName, typeName, secName);
        }
    }

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

