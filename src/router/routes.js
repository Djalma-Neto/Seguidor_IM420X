const routes = [
  {
    path: "/",
    component: () => import("layouts/defaultLayout.vue"),
    children: [
      { path: "/", component: () => import("pages/Index.vue"),name:"login" }
    ]
  },
  {
    path: "/dashboard",
    component: () => import("layouts/defaultLayout.vue"),
    children: [
      { path: "/", component: () => import("pages/dashBoard.vue"),name:"dashBoard" }
    ]
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: "/:catchAll(.*)*",
    component: () => import("pages/Error404.vue"),
  },
];

export default routes;
