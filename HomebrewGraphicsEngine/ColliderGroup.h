#pragma once
#include "Collider.h"

namespace hograengine {
	class ColliderGroup
	{
	public:

		void addCollider(Collider* collider) {
			colliders.push_back(collider);
		}

		void removeCollider(Collider* collider) {
			const auto& iter = std::find(colliders.begin(), colliders.end(), collider);
			if (iter != colliders.end()) {
				colliders.erase(iter);
			}
		}

		const std::vector<Collider*>& getColliders() const {
			return colliders;
		}

		void selfCollide() {
			if (ColliderGroupType::collide_only_with_group_members == type || ColliderGroupType::collide_with_all == type) {
				for (int i = 0; i < colliders.size() - 1; i++) {
					for (int j = i + 1; j < colliders.size(); j++) {
						colliders[i]->collide(*colliders[j]);
					}
				}
			}
		}

		void collide(ColliderGroup* otherGroup) {
			const auto otherType = otherGroup->getType();
			if ((ColliderGroupType::collide_only_with_non_group_members == type || ColliderGroupType::collide_with_all == type)
				&& (ColliderGroupType::collide_only_with_non_group_members == otherType || ColliderGroupType::collide_with_all == otherType)) {
				auto& otherColliders = otherGroup->getColliders();
				for (int i = 0; i < colliders.size(); i++) {
					for (int j = 0; j < otherColliders.size(); j++) {
						colliders[i]->collide(*otherColliders[j]);
					}
				}
			}
		}

		enum class ColliderGroupType {
			collide_with_all,
			collide_only_with_non_group_members,
			collide_only_with_group_members,
		};

		const ColliderGroupType& getType() const {
			return type;
		}

		void setType(ColliderGroupType _type) {
			type = _type;
		}

	private:
		ColliderGroupType type = ColliderGroupType::collide_with_all;
		std::vector<Collider*> colliders;

	};
}

