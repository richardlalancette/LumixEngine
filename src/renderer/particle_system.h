#pragma once


#include "engine/lumix.h"
#include "engine/array.h"
#include "engine/blob.h"
#include "engine/resource.h"
#include "engine/resource_manager.h"
#include "engine/simd.h"


namespace Lumix
{


class Material;


class ParticleEmitterResourceManager final : public ResourceManager
{
public:
	ParticleEmitterResourceManager(IAllocator& allocator) 
		: ResourceManager(allocator)
		, m_allocator(allocator) {}

	Resource* createResource(const Path& path) override;
	void destroyResource(Resource& resource) override;
private:
	IAllocator& m_allocator;
};


class ParticleEmitterResource final : public Resource
{
public:
	static const ResourceType TYPE;

	ParticleEmitterResource(const Path& path, ResourceManager& manager, IAllocator& allocator);

	ResourceType getType() const override { return TYPE; }
	void unload() override;
	bool load(FS::IFile& file) override;
	const OutputBlob& getBytecode() const { return m_bytecode; }
	int getEmitByteOffset() const { return m_emit_byte_offset; }
	int getOutputByteOffset() const { return m_output_byte_offset; }
	int getChannelsCount() const { return m_channels_count; }
	int getRegistersCount() const { return m_registers_count; }
	int getOutputsCount() const { return m_outputs_count; }
	Material* getMaterial() const { return m_material; }
	void setMaterial(const Path& path);

private:
	OutputBlob m_bytecode;
	int m_emit_byte_offset;
	int m_output_byte_offset;
	int m_channels_count;
	int m_registers_count;
	int m_outputs_count;
	Material* m_material;
};



class Material;
class ResourceManagerHub;


class LUMIX_RENDERER_API ParticleEmitter
{
public:
	ParticleEmitter(EntityPtr entity, IAllocator& allocator);
	~ParticleEmitter();

	void serialize(OutputBlob& blob);
	void deserialize(InputBlob& blob, ResourceManagerHub& manager);
	void update(float dt);
	void emit(const float* args);
	void fillInstanceData(const Vec3& cam_pos, float* data);
	int getInstanceDataSizeBytes() const;
	ParticleEmitterResource* getResource() const { return m_resource; }
	void setResource(ParticleEmitterResource* res);
	int getInstancesCount() const { return m_instances_count; }
	float* getChannelData(int idx) const { return m_channels[idx].data; }
	
	EntityPtr m_entity;

private:
	struct Channel
	{
		float* data = nullptr;
		u32 name = 0;
	};

	struct Constant
	{
		u32 name = 0;
		float value = 0;
	};

	void execute(InputBlob& blob, int particle_index);
	void kill(int particle_index);
	float readSingleValue(InputBlob& blob) const;

	IAllocator& m_allocator;
	OutputBlob m_emit_buffer;
	Constant m_constants[16];
	int m_constants_count = 0;
	Channel m_channels[16];
	int m_capacity = 0;
	int m_outputs_per_particle = 0;
	int m_particles_count = 0;
	int m_instances_count = 0;
	ParticleEmitterResource* m_resource = nullptr;
};


} // namespace Lumix