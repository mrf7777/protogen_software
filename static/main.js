console.log("Hello, World!")

const origin = window.location.origin

let normal_button = document.getElementById("normal-button")
normal_button.addEventListener("click", (e) => {
	console.log("normal clicked")
	fetch(`${origin}/protogen/head/emotion`, {method: "put", body: "normal"})	
})
