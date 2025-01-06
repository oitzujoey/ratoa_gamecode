#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "bg_local.h"


qboolean BG_TeleportPlayer( playerState_t *playerState, trace_t *entranceTrace, vec3_t entranceOrigin, vec3_t exitOrigin, vec3_t exitAngles, qboolean silentTeleport ) {
	qboolean noAngles = (exitAngles[0] > 999999.0);
	if (!noAngles) {
		if (silentTeleport) {
			vec3_t zero;

			vec3_t entry_normal;
			vec3_t entry_angles;
			vec3_t angleDifference;

			vec3_t projectedOrigin;
			vec3_t projectedEntranceBrushOrigin;
			vec3_t entrancePositionOffset;
			vec3_t exitPositionOffset;
			vec3_t entrancePositionAngles;
			vec3_t exitPositionAngles;
			vec_t originalOffset;

			vec_t originalSpeed;
			vec3_t velocityAngles;

			VectorClear(zero);

			// TODO: Use a rotation matrix.

			// Required z-axis rotation:
			VectorCopy(entranceTrace->plane.normal, entry_normal);
			// Negation not needed for some reason.
			VectorNegate(entry_normal, entry_normal);
			vectoangles(entry_normal, entry_angles);
			AnglesSubtract(exitAngles, entry_angles, angleDifference);

			// Rotate view angles:
			VectorAdd(playerState->viewangles, angleDifference, exitAngles);
			AnglesSubtract(exitAngles, zero, exitAngles);

			// Rotate position:
			//entranceTrace->endpos
			/* Com_Printf("playerState->origin %f %f %f\n", playerState->origin[0], playerState->origin[1], playerState->origin[2]); */
			/* Com_Printf("entranceOrigin %f %f %f\n", entranceOrigin[0], entranceOrigin[1], entranceOrigin[2]); */
			ProjectPointOnPlane(projectedOrigin, playerState->origin, entry_normal);
			/* Com_Printf("projectedOrigin %f %f %f\n", projectedOrigin[0], projectedOrigin[1], projectedOrigin[2]); */
			ProjectPointOnPlane(projectedEntranceBrushOrigin, entranceOrigin, entry_normal);
			/* Com_Printf("projectedEntranceBrushOrigin %f %f %f\n", projectedEntranceBrushOrigin[0], projectedEntranceBrushOrigin[1], projectedEntranceBrushOrigin[2]); */
			VectorSubtract(projectedOrigin, projectedEntranceBrushOrigin, entrancePositionOffset);
			/* Com_Printf("entrancePositionOffset %f %f %f\n", entrancePositionOffset[0], entrancePositionOffset[1], entrancePositionOffset[2]); */
			vectoangles(entrancePositionOffset, entrancePositionAngles);
			VectorAdd(entrancePositionAngles, angleDifference, exitPositionAngles);
			originalOffset = VectorLength(entrancePositionOffset);
			/* Com_Printf("originalOffset %f\n", originalOffset); */
			AngleVectors(exitPositionAngles, exitPositionOffset, NULL, NULL);
			VectorScale(exitPositionOffset, originalOffset, exitPositionOffset);
			/* Com_Printf("exitPositionOffset %f %f %f\n", exitPositionOffset[0], exitPositionOffset[1], exitPositionOffset[2]); */
			VectorAdd(exitOrigin, exitPositionOffset, exitOrigin);
			/* exitOrigin[2] += MINS_Z; */
			/* Com_Printf("exitOrigin %f %f %f\n", exitOrigin[0], exitOrigin[1], exitOrigin[2]); */

			// Rotate velocity:
			vectoangles(playerState->velocity, velocityAngles);
			VectorAdd(velocityAngles, angleDifference, velocityAngles);
			AnglesSubtract(velocityAngles, zero, velocityAngles);
			originalSpeed = VectorLength(playerState->velocity);
			AngleVectors( velocityAngles, playerState->velocity, NULL, NULL );
			VectorScale( playerState->velocity, originalSpeed, playerState->velocity );
		}
		else {
			AngleVectors( exitAngles, playerState->velocity, NULL, NULL );
			VectorScale( playerState->velocity, 400, playerState->velocity );
			playerState->pm_time = 160;  // hold time
			playerState->pm_flags |= PMF_TIME_KNOCKBACK;
		}
	}
	return !noAngles;
}
