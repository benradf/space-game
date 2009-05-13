/// \file graphics.cpp
/// \brief Handles client graphics.
/// \author Ben Radford 
/// \date 24th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "graphics.hpp"
#include <core/core.hpp>


////////// MovableParticleSystem //////////

gfx::MovableParticleSystem::MovableParticleSystem(const char* name, const char* system, 
        const Ogre::Vector3& offset, Ogre::SceneNode* node, Ogre::SceneManager* sceneManager) :
    _sceneManager(sceneManager), _system(0), _node(0), _offset(offset)
{
    _system = _sceneManager->createParticleSystem(name, system);
    _node = node->createChildSceneNode();

    _node->attachObject(_system);
    _node->setPosition(_offset);
    _node->setInheritOrientation(false);

    for (int i = 0; i < _system->getNumEmitters(); i++) {
        Ogre::ParticleEmitter* emitter = _system->getEmitter(i);
        _dir.push_back(emitter->getDirection() * emitter->getParticleVelocity());
    }

    _system->setKeepParticlesInLocalSpace(true);
}

gfx::MovableParticleSystem::~MovableParticleSystem()
{
    _sceneManager->destroyParticleSystem(_system);
}

void gfx::MovableParticleSystem::update(const Ogre::Vector3& velocity)
{
    for (int i = 0; i < _system->getNumEmitters(); i++) {
        Ogre::SceneNode* parent = _node->getParentSceneNode();
        Ogre::Vector3 dir = parent->getOrientation() * _dir[i];

        Ogre::Real mag = dir.length();
        dir /= mag;

        _system->getEmitter(i)->setParticleVelocity(mag);
        _system->getEmitter(i)->setDirection(dir);
    }
}

Ogre::ParticleSystem& gfx::MovableParticleSystem::get()
{
    assert(_system != 0);

    return *_system;
}

void gfx::MovableParticleSystem::setEnabled(bool enabled)
{
    assert(_system != 0);

    for (int i = 0; i < _system->getNumEmitters(); i++) 
        _system->getEmitter(i)->setEnabled(enabled);
}

void gfx::MovableParticleSystem::setVisible(bool visible)
{
    assert(_system != 0);

    _system->setVisible(visible);
}

void gfx::MovableParticleSystem::setPaused(bool paused)
{
    assert(_system != 0);

    _system->setSpeedFactor(paused ? 0.0f : 1.0f);
}


////////// Entity //////////

gfx::Entity::Entity(const char* name, const char* mesh, Ogre::SceneManager* sceneManager) :
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

gfx::Entity::~Entity()
{
    foreach (MovableParticleSystem* system, _particleSystems) 
        std::auto_ptr<MovableParticleSystem>(system);

    _node->detachObject(_entity);
    _sceneManager->destroyEntity(_entity);
    _sceneManager->getRootSceneNode()->removeAndDestroyChild(_name);
}

void gfx::Entity::setPosition(const Ogre::Vector3& pos)
{
    _node->setPosition(pos);
}

const Ogre::Vector3& gfx::Entity::getPosition() const
{
    return _node->getPosition();
}

void gfx::Entity::setOrientation(const Ogre::Quaternion& rot)
{
    _node->setOrientation(rot);
}

const Ogre::Quaternion& gfx::Entity::getOrientation() const
{
    return _node->getOrientation();
}

void gfx::Entity::setMaterial(const char* material) const
{
    _entity->setMaterialName(material);
}

gfx::MovableParticleSystem* gfx::Entity::attachParticleSystem(
    const char* system, const Ogre::Vector3& offset)
{
    char name[64];
    snprintf(name, sizeof(name), "_particlesystem_%s_%s_%d", 
        _name.c_str(), system, int(_particleSystems.size() + 1));

    std::auto_ptr<MovableParticleSystem> particleSystem(
        new MovableParticleSystem( name, system, 
        offset, _node, _sceneManager));

    _particleSystems.push_back(particleSystem.get());

    return particleSystem.release();
}

void gfx::Entity::updateParticleSystems(const Ogre::Vector3& velocity)
{
    foreach (MovableParticleSystem* system, _particleSystems) 
        system->update(velocity);
}


////////// Camera //////////

gfx::Camera::Camera(const char* name, Ogre::SceneManager* sceneManager) :
    _camera(0), _sceneManager(sceneManager)
{
    _camera = _sceneManager->createCamera(name);
    _camera->setNearClipDistance(1.0f);
    _camera->setFarClipDistance(50000.0f);
}

gfx::Camera::~Camera()
{
    _sceneManager->destroyCamera(_camera);
}

void gfx::Camera::setPosition(const Ogre::Vector3& pos)
{
    _camera->setPosition(pos);
}

const Ogre::Vector3& gfx::Camera::getPosition() const
{
    return _camera->getPosition();
}

void gfx::Camera::lookAt(const Ogre::Vector3& pos)
{
    _camera->lookAt(pos);
}


////////// Backdrop //////////

gfx::Backdrop::Backdrop(const char* name, const char* material, float scroll, 
        float depth, float size, Ogre::SceneManager* sceneManager) :
    _scroll(scroll), _depth(depth), _size(size)
{
    _mesh = Ogre::MeshManager::getSingleton().createPlane(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::Plane(Ogre::Vector3::UNIT_Z, Ogre::Real(0.0f)),
        Ogre::Real(3.0f * size), Ogre::Real(3.0f * size), 3, 3, 
        false, 1, Ogre::Real(3.0f), Ogre::Real(3.0f));

    if (_mesh.isNull()) 
        throw MemoryException("failed to create backdrop mesh");

    try {
        _entity.reset(new Entity(name, name, sceneManager));
        _entity->setMaterial(material);
        update(Ogre::Vector3::ZERO);
    } catch (...) {
        Ogre::MeshManager::getSingleton().remove(_mesh->getHandle());
        throw;
    }
}

gfx::Backdrop::~Backdrop()
{
    Ogre::MeshManager::getSingleton().remove(_mesh->getHandle());
}

#include <iostream>
using namespace std;
void gfx::Backdrop::update(const Ogre::Vector3& centre)
{
    float halfSize = 0.5f * _size;

    Ogre::Vector3 offset = -_scroll * centre;
    offset.z = 0.0f;

    offset.x -= halfSize;
    offset.x = fmod(offset.x, _size);
    if (offset.x < -halfSize) 
        offset.x += _size;
    if (offset.x > halfSize) 
        offset.x -= _size;
    offset.x += halfSize;

    offset.y -= halfSize;
    offset.y = fmod(offset.y, _size);
    if (offset.y < -halfSize) 
        offset.y += _size;
    if (offset.y > halfSize) 
        offset.y -= _size;
    offset.y += halfSize;

    Ogre::Vector3 pos = centre + offset;
    pos.z = _depth;

    _entity->setPosition(pos);
}


////////// Scene //////////

gfx::Scene::Scene(boost::shared_ptr<Ogre::Root> root) :
    _root(root), _sceneManager(0)
{
    _sceneManager = _root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "ExteriorSceneManager");
    //_sceneManager->setDisplaySceneNodes(true);
}

gfx::Scene::~Scene()
{
    _root->destroySceneManager(_sceneManager);
}

std::auto_ptr<gfx::Camera> gfx::Scene::createCamera(const char* name)
{
    return std::auto_ptr<Camera>(new Camera(name, _sceneManager));
}

std::auto_ptr<gfx::Entity> gfx::Scene::createEntity(const char* name, const char* mesh)
{
    return std::auto_ptr<Entity>(new Entity(name, mesh, _sceneManager));
}

void gfx::Scene::setSkyPlane(const char* material, const Ogre::Vector3& normal, float dist)
{
    _sceneManager->setSkyPlane(true, Ogre::Plane(normal, Ogre::Real(dist)), material,
        Ogre::Real(500.0f), Ogre::Real(100.0f));
}

void gfx::Scene::addBackdrop(const char* material, float scroll, float depth, float size)
{
    char name[16];
    snprintf(name, sizeof(name), "_backdrop%02d", int(_backdrops.size() + 1));
    printf("backdrop name = '%s'\n", name);

    std::auto_ptr<Backdrop> backdrop(new Backdrop(name, 
        material, scroll, depth, size, _sceneManager));
    _backdrops.push_back(backdrop.get());
    backdrop.release();
}

void gfx::Scene::updateBackdropPositions(const Ogre::Vector3& centre)
{
    foreach (Backdrop* backdrop, _backdrops) 
        backdrop->update(centre);
}


////////// Viewport //////////

gfx::Viewport::Viewport(const char* title, boost::shared_ptr<Ogre::Root> root) :
    _root(root), _window(0), _viewport(0), _camera(0)
{
    _window = _root->initialise(true, title);

#if defined(WIN32)
    HWND hwnd;
    _window->getCustomAttribute("WINDOW", reinterpret_cast<void*>(&hwnd));
    LONG iconID = LONG(LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(101)));
    SetClassLong(hwnd, GCL_HICON, iconID);
#endif
}

gfx::Viewport::~Viewport()
{
    _window->removeAllViewports();
    _root->detachRenderTarget(_window);
}

void gfx::Viewport::update()
{
    _window->update();
}

void gfx::Viewport::attachCamera(Camera& camera)
{
    _window->removeAllViewports();
    _camera = camera._camera;
    _viewport = _window->addViewport(_camera);
    _window->update();

    updateAspectRatio();
}

Ogre::RenderWindow* gfx::Viewport::getRenderWindow()
{
    return _window;
}

void gfx::Viewport::updateAspectRatio()
{
    Ogre::Real width = _viewport->getActualWidth();
    Ogre::Real height = _viewport->getActualHeight();
    _camera->setAspectRatio(width / height);
}


////////// GFXManager //////////

#include <iostream>
using namespace std;
gfx::GFXManager::GFXManager()
{
    _root.reset(new Ogre::Root());

    //_root->showConfigDialog();
    _root->restoreConfig();

    _viewport.reset(new Viewport("mmoclient", _root));

    initResources();
}

gfx::GFXManager::~GFXManager()
{

}

void gfx::GFXManager::render()
{
    _root->renderOneFrame();
}

gfx::Viewport& gfx::GFXManager::getViewport()
{
    return *_viewport;
}

std::auto_ptr<gfx::Scene> gfx::GFXManager::createScene()
{
    return std::auto_ptr<Scene>(new Scene(_root));
}

void gfx::GFXManager::initResources()
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

