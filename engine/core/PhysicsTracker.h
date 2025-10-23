#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iostream>
#include "MathLibrary.h"

// Forward declarations
class Actor;
class GraphicNode;

namespace Physics {

    // Structure to hold object state at a specific time step
    struct ObjectState {
        // Unique identifier for the object
        size_t objectId;
        
        // Transform and motion data
        M4 transform;
        M4 rotation;
        V4 linearVelocity;
        V4 angularVelocity;
        
        // Physical properties
        float mass;
        float elasticity;
        float drag;
        bool isDynamic;
        
        // Position and orientation
        V3 position;
        V3 centerOfMass;
        Quaternion orientation;
        
        // AABB for collision detection
        V3 aabbMin;
        V3 aabbMax;
        
        // Forces applied this step
        V3 appliedForces;
        float angle;
        float angleVel;
        
        // Collision status
        bool wasInCollision;
        size_t collisionPartner; // ID of colliding object, SIZE_MAX if none
        
        ObjectState() : objectId(SIZE_MAX), mass(0.0f), elasticity(0.0f), drag(0.0f), 
                       isDynamic(false), angle(0.0f), angleVel(0.0f), 
                       wasInCollision(false), collisionPartner(SIZE_MAX) {}
    };

    // Structure to hold collision event information
    struct CollisionEvent {
        size_t frameIndex;
        size_t objectId1;
        size_t objectId2;
        
        // Collision details
        V3 collisionPoint;
        V3 collisionNormal;
        float penetrationDepth;
        float relativeVelocity;
        
        // Pre-collision states
        ObjectState preCollisionState1;
        ObjectState preCollisionState2;
        
        // Post-collision states
        ObjectState postCollisionState1;
        ObjectState postCollisionState2;
        
        // Collision response info
        V4 impulse;
        float restitutionUsed;
        
        // Timing
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    // Structure to hold performance data for each frame
    struct FramePerformance {
        size_t frameIndex;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        
        // Timing breakdowns (in microseconds)
        float totalFrameTime;
        float physicsUpdateTime;
        float collisionDetectionTime;
        float collisionResponseTime;
        float renderTime;
        
        // Statistics
        size_t activeObjects;
        size_t dynamicObjects;
        size_t collisionChecks;
        size_t actualCollisions;
        
        FramePerformance() : frameIndex(0), totalFrameTime(0.0f), physicsUpdateTime(0.0f),
                           collisionDetectionTime(0.0f), collisionResponseTime(0.0f), 
                           renderTime(0.0f), activeObjects(0), dynamicObjects(0),
                           collisionChecks(0), actualCollisions(0) {}
    };

    // Main physics tracking class
    class PhysicsTracker {
    private:
        // Storage for all tracking data
        std::vector<std::vector<ObjectState>> frameStates; // [frame][objectId] = state
        std::vector<CollisionEvent> collisionHistory;
        std::vector<FramePerformance> performanceHistory;
        
        // Current frame tracking
        size_t currentFrame;
        bool isTracking;
        bool trackingEnabled;
        
        // Object ID mapping
        std::unordered_map<void*, size_t> objectToIdMap;
        std::vector<std::shared_ptr<GraphicNode>> trackedObjects;
        size_t nextObjectId;
        
        // Performance timing helpers
        std::chrono::high_resolution_clock::time_point frameStartTime;
        std::chrono::high_resolution_clock::time_point lastTimestamp;
        
        // Configuration
        bool exportOnDestroy;
        std::string outputDirectory;
        size_t maxStoredFrames;
        
    public:
        PhysicsTracker(size_t maxFrames = 10000);
        ~PhysicsTracker();
        
        // Tracking control
        void StartTracking();
        void StopTracking();
        void PauseTracking();
        void ResumeTracking();
        bool IsTracking() const { return isTracking && trackingEnabled; }
        void ClearHistory();
        
        // Object management
        size_t RegisterObject(std::shared_ptr<GraphicNode> object);
        void UnregisterObject(size_t objectId);
        void UnregisterObject(std::shared_ptr<GraphicNode> object);
        
        // Step tracking
        void BeginFrame();
        void EndFrame();
        void RecordObjectState(size_t objectId, std::shared_ptr<GraphicNode> object);
        void RecordObjectState(std::shared_ptr<GraphicNode> object);
        void RecordCollision(const CollisionEvent& collision);
        
        // Performance tracking
        void StartPhysicsUpdate();
        void EndPhysicsUpdate();
        void StartCollisionDetection();
        void EndCollisionDetection();
        void StartCollisionResponse();
        void EndCollisionResponse();
        void StartRender();
        void EndRender();
        
        // Data access
        const std::vector<ObjectState>& GetFrameStates(size_t frameIndex) const;
        const std::vector<CollisionEvent>& GetCollisionHistory() const { return collisionHistory; }
        const std::vector<FramePerformance>& GetPerformanceHistory() const { return performanceHistory; }
        
        // Statistics
        size_t GetTotalFrames() const { return frameStates.size(); }
        size_t GetTotalCollisions() const { return collisionHistory.size(); }
        size_t GetTrackedObjectCount() const { return trackedObjects.size(); }
        
        // Export functionality
        void ExportToCSV(const std::string& filename) const;
        void ExportToJSON(const std::string& filename) const;
        void ExportObjectTrajectory(size_t objectId, const std::string& filename) const;
        void ExportCollisionReport(const std::string& filename) const;
        void ExportPerformanceReport(const std::string& filename) const;
        
        // Analysis helpers
        float GetAverageFrameTime() const;
        float GetAverageCollisionsPerFrame() const;
        std::vector<V3> GetObjectTrajectory(size_t objectId) const;
        void PrintSummary() const;
        
        // Configuration
        void SetMaxStoredFrames(size_t maxFrames) { maxStoredFrames = maxFrames; }
        void SetOutputDirectory(const std::string& dir) { outputDirectory = dir; }
        void SetExportOnDestroy(bool enable) { exportOnDestroy = enable; }
        
    private:
        // Helper methods
        ObjectState CaptureObjectState(size_t objectId, std::shared_ptr<GraphicNode> object);
        void CleanupOldFrames();
        std::string GetTimestampString() const;
        float GetElapsedTime(const std::chrono::high_resolution_clock::time_point& start,
                           const std::chrono::high_resolution_clock::time_point& end) const;
    };

} // namespace Physics
