let player_id = (new URLSearchParams(window.location.search)).get("player_id")

document.getElementById("minecraft-left").addEventListener("click", () => {
	playerMove(player_id, "left")
})
document.getElementById("minecraft-right").addEventListener("click", () => {
	playerMove(player_id, "right")
})
document.getElementById("minecraft-up").addEventListener("click", () => {
	playerMove(player_id, "up")
})
document.getElementById("minecraft-down").addEventListener("click", () => {
	playerMove(player_id, "down")
})

document.getElementById("place-block").addEventListener("click", () => {
	playerPlaceBlock(player_id)
})
